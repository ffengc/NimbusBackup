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
        // nimbus::HotManager hot;
        // hot.Run();
        auto back_dir = conf->back_dir();
        auto pack_dir = conf->pack_dir();
        FileUtil tmp1(back_dir);
        FileUtil tmp2(pack_dir);
        tmp1.create_dir();
        tmp2.create_dir();

    }
    bool Run() {
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
        return true;
    } //
private:
    void __upload(const httplib::Request& req, httplib::Response& rsp) {
        LOG(INFO) << "get a upload request, call __upload" << std::endl;
        auto ret = req.has_file("file"); // 判断有没有上传的文件区域
        if (ret == false) {
            LOG(WARNING) << "no file upload" << std::endl;
            rsp.status = 400;
            return;
        }
        const auto& file = req.get_file_value("file");
        // file.filename; // 文件名称
        // file.content; // 文件数据
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
    void __download(const httplib::Request& req, httplib::Response& rsp) { }
};
} // namespace nimbus

#endif