/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// client.hpp

#ifndef __YUFC_CLIENT_HPP__
#define __YUFC_CLIENT_HPP__

#include "../tools/httplib.h"
#include "data_manager.hpp"
#include <sstream>
#include <string>
#include <vector>

namespace nimbus {
#define SERVER_IP "10.211.55.4"
#define SERVER_PORT 8080
#define TIME_TO_UPLOAD 10 // 文件多久没有被修改, 则可以上传
class BackUp {
private:
    std::string __back_dir;
    DataManager* __data; //
public:
    BackUp(const std::string& back_dir, const std::string& back_file)
        : __back_dir(back_dir) {
        __data = new DataManager(back_file);
    } //
private:
    std::string __generate_file_identifier(const std::string& file_name) {
        // 创建文件的唯一标识
        // a.txt-size-mtime
        FileUtil fu(file_name);
        std::stringstream ss;
        ss << fu.file_name() << "-" << fu.file_size() << "-" << fu.last_modify_time();
        return ss.str();
    }
    bool __upload(const std::string& filename) {
        // 1. 获取文件数据
        FileUtil fu(filename);
        if (!fu.exists()) {
            LOG(ERROR) << "file: " << filename << "not exists, cannot be uploaded" << std::endl;
            return false;
        }
        std::string body;
        fu.access_content(&body);
        // 2. 搭建http客户端上传文件数据
        httplib::Client cli(SERVER_IP, SERVER_PORT);
        httplib::MultipartFormData item;
        item.content = body;
        item.filename = fu.file_name();
        item.name = "file";
        item.content_type = "application/octet-stream"; // 二进制流数据

        httplib::MultipartFormDataItems items;
        items.push_back(item);
        auto res = cli.Post("/upload", items);
        if (!res || res->status != 200) {
            LOG(ERROR) << "upload error" << std::endl;
            return false;
        }
        return true;
    }
    bool __check_upload(const std::string& file_name) {
        // 判断一个文件是否需要上传
        // 文件是新增的 或 不是新增的但是被修改过
        std::string id;
        if (__data->get(file_name, &id)) {
            // 有历史信息
            std::string new_id = __generate_file_identifier(file_name);
            if (new_id == id) // 没有被修改过
                return false;
        }
        // 考虑一个情况, 正在缓慢拷贝到这个目录下, 拷贝需要过程
        // 因此, 要判断, 有一段时间没有被修改了, 才能上传! 这是本项目简单的实现方式
        // 更合理的方式应该是: 判断当前文件是否被正在使用或打开
        FileUtil fu(file_name);
        if (time(NULL) - fu.last_modify_time() < TIME_TO_UPLOAD)
            return false;
        return true;
    } //
public:
    bool run() {
        LOG(INFO) << "client running ..." << "\n\r" << "BACKDIR: " << __back_dir << std::endl;
        while (true) {
            FileUtil fu(__back_dir);
            fu.create_dir();
            std::vector<std::string> arry;
            fu.scan_dir(&arry);
            for (const auto& e : arry) {
                // 遍历每一个文件, 判断这个文件是否需要上传
                if (__check_upload(e) == false)
                    continue;
                if (__upload(e)) { // 上传这个文件
                    __data->insert(e, __generate_file_identifier(e)); // 新增文件备份信息
                    LOG(INFO) << "upload: " << e << " success" << std::endl;
                } else {
                    LOG(WARNING) << "upload: " << e << " error" << std::endl;
                }
            }
            usleep(1000);
        }
    }
};
} // namespace nimbus

#endif