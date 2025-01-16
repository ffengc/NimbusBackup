/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// util.hpp

#ifndef __YUFC_UTIL_HPP__
#define __YUFC_UTIL_HPP__

#include "logger.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <vector>

namespace nimbus {
class FileUtil {
private:
    std::string __file_name; // 、
public:
    FileUtil(const std::string& file_name)
        : __file_name(file_name) { }
    // 获取文件大小
    int64_t file_size() {
        struct stat st;
        if (stat(__file_name.c_str(), &st) < 0) {
            LOG(ERROR) << "get file size failed!" << std::endl;
            return -1;
        }
        return st.st_size;
    }
    // 获取文件最后一次修改时间
    time_t last_modify_time() {
        struct stat st;
        if (stat(__file_name.c_str(), &st) < 0) {
            LOG(ERROR) << "get file last modify time failed!" << std::endl;
            return -1;
        }
        return st.st_mtime;
    }
    // 获取文件最后一次被访问时间
    time_t last_access_time() {
        struct stat st;
        if (stat(__file_name.c_str(), &st) < 0) {
            LOG(ERROR) << "get file last access time failed!" << std::endl;
            return -1;
        }
        return st.st_atime;
    }
    // 获取文件路径中的文件名
    // ./abc/test.txt -> test.txt
    std::string file_name() {
        size_t pos = __file_name.find_last_of("/");
        if (pos == std::string::npos)
            return __file_name;
        return __file_name.substr(pos + 1);
    }
    // 获取文件指定位置内容
    bool access_pos_content(std::string* body, size_t pos, size_t len) {
        std::ifstream ifs;
        ifs.open(__file_name, std::ios::binary); // 读取
        if (ifs.is_open() == false) {
            LOG(ERROR) << "open file failed" << std::endl;
            return false;
        }
        // 获取文件数据
        size_t fsize = this->file_size();
        // 如果 pos + len 太长 则读取错误
        if (pos + len > fsize) {
            LOG(ERORR) << "pos error, file overflow" << std::endl;
            return false;
        }
        ifs.seekg(pos, std::ios::beg);
        body->resize(len);
        ifs.read(&(*body)[0], len);
        if (!ifs.good()) {
            LOG(ERROR) << "read file error" << std::endl;
            return false;
        }
        ifs.close();
        return true;
    }
    // 获取文件内容
    bool access_content(std::string* body) {
        size_t fsize = this->file_size();
        return this->access_pos_content(body, 0, fsize);
    }
    // 设置文件内容
    bool set_content(const std::string& body) {
        std::ofstream ofs;
        ofs.open(__file_name, std::ios::binary);
        if (ofs.is_open() == false) {
            LOG(ERROR) << "open file failed" << std::endl;
            return false;
        }
        ofs.write(&body[0], body.size());
        if (ofs.good() == false) {
            LOG(ERROR) << "write to file failed" << std::endl;
            ofs.close();
            return false;
        }
        ofs.close();
        return true;
    }
};
} // namespace nimbus

#endif