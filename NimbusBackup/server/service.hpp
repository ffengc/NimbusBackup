/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// service.hpp

#ifndef __YUFC_SERVICE_HPP__
#define __YUFC_SERVICE_HPP__

#include "../tools/httplib.h"
#include "data_manager.hpp"
#include "hot.hpp"
#include "view.hpp"
#include <functional>
#include <sstream>
#include <thread>

extern nimbus::DataManager* __data; // 数据管理全局变量
namespace nimbus {
class Service {
private:
    int __server_port;
    std::string __server_ip;
    std::string __download_prefix;
    httplib::Server __server;
    std::string __wwwroot; //
public:
    Service() {
        Config* conf = Config::get_instance();
        __server_port = conf->server_port();
        __server_ip = conf->server_ip();
        __download_prefix = conf->download_prefix();
        __wwwroot = conf->wwwroot();
        auto back_dir = conf->back_dir();
        auto pack_dir = conf->pack_dir();
        FileUtil tmp1(back_dir);
        FileUtil tmp2(pack_dir);
        tmp1.create_dir();
        tmp2.create_dir();
    }
    void Run() {
        std::thread hot(__hot_run);
        __server_run();
        hot.join();
    } //
private:
    void __server_run() {
        __server.set_base_dir(__wwwroot.c_str());
        __server.Post("/upload", static_cast<httplib::Server::Handler>(std::bind(&Service::__upload, this, std::placeholders::_1, std::placeholders::_2)));
        __server.Get("/listshow", std::bind(&Service::__show, this, std::placeholders::_1, std::placeholders::_2));
        __server.Get("/", std::bind(&Service::__show, this, std::placeholders::_1, std::placeholders::_2));
        std::string downlaod_url = __download_prefix + "(.*)"; // 正则表达式
        __server.Get(downlaod_url, std::bind(&Service::__download, this, std::placeholders::_1, std::placeholders::_2)); // 正则表达式
        LOG(INFO) << "server listening in ip: " << __server_ip
                  << ", port: " << std::to_string(__server_port)
                  << std::endl;
        __server.listen(__server_ip.c_str(), __server_port);
    }
    static void __hot_run() {
        nimbus::HotManager hot;
        hot.Run();
    } //
private:
    void __upload(const httplib::Request& req, httplib::Response& rsp) {
        auto ret = req.has_file("file"); // 判断有没有上传的文件区域
        if (ret == false) {
            LOG(WARNING) << "no file upload" << std::endl;
            rsp.status = 400;
            return;
        }
        const auto& file = req.get_file_value("file");
        // file.filename; // 文件名称
        // file.content; // 文件数据
        LOG(REQUEST) << "get a upload request: " << file.filename << std::endl;
        std::string back_dir = Config::get_instance()->back_dir();
        std::string real_path = back_dir + FileUtil(file.filename).file_name();
        FileUtil fu(real_path);
        fu.set_content(file.content); // 将数据写入文件中
        __backup_info info;
        info.new_backup_info(real_path); // 组织文件信息
        __data->insert(info); // 向数据管理模块添加备份信息
    }
    void __show(const httplib::Request& req, httplib::Response& rsp) {
        // 1. 获取所有的文件备份信息
        std::vector<__backup_info> arry;
        __data->access_all(&arry);
        // 2. 根据所有备份信息, 组织html文件数据
        auto res_html = generate_html(arry);
        rsp.body = res_html;
        rsp.set_header("Content-Type", "text/html");
        rsp.status = 200;
    }
    void __download(const httplib::Request& req, httplib::Response& rsp) {
        // 1. 获取客户端请求的资源路径 path
        LOG(REQUEST) << "client download " << req.path << std::endl;
        // 2. 根据资源路径, 获取文件备份信息
        __backup_info info;
        __data->access_info_by_url(req.path, &info);
        // 3. 判断文件是否被压缩, 如果被压缩, 要先解压缩
        if (info.pack_flag__ == true) {
            // 3.1 删除压缩包, 修改备份信息
            FileUtil pack_file(info.pack_path__);
            pack_file.unpack(info.real_path__);
            pack_file.remove();
            info.pack_flag__ = false;
            __data->update(info);
        }
        // *判断是否需要断点续传
        FileUtil fu(info.real_path__);
        bool retrans_flag = false; // 是否需要断点续传
        std::string old_etag; // 原先的etag
        if (req.has_header("If-Range")) {
            old_etag = req.get_header_value("If-Range");
            if (old_etag == this->__generate_etag(info))
                retrans_flag = true; // 需要断点续传
        }
        // 如果没有 "If-Range" 则是正常下载, 如果有 "If-Range", 但 etag 变了, 也是正常下载
        // 4. 读取文件数据, 放入 rsp.body 中
        // 5. 设置响应头部字段 ETag, Accept-Ranges
        if (retrans_flag == false) {
            fu.access_content(&rsp.body);
            rsp.set_header("Accept-Ranges", "bytes");
            rsp.set_header("ETag", __generate_etag(info));
            rsp.set_header("Content-Type", "application/octet-stream");
            rsp.status = 200;
        } else {
            // 断点续传 httplib 其实里面已经帮我们解析了 body 了, 我们只需要将文件所有数据读进来 rsp.body, 给个新的 etag 即可
            // httplib 内部会自动根据请求区间, 从 body 中截取需要的数据来进行响应
            // 所以其实不用 if else 都是可以的!
            fu.access_content(&rsp.body);
            rsp.set_header("Accept-Ranges", "bytes");
            rsp.set_header("ETag", __generate_etag(info));
            rsp.set_header("Content-Type", "application/octet-stream");
            rsp.status = 206; // 区间请求响应的是 206
        }
    } //
private:
    std::string __generate_etag(const __backup_info& info) {
        FileUtil fu(info.real_path__);
        std::stringstream ss;
        ss << fu.file_name() << "-"
           << std::to_string(info.file_size__)
           << "-" << std::to_string(info.last_modify_time__);
        return ss.str();
    }
};
} // namespace nimbus

#endif