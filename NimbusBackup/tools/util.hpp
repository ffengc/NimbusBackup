/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// util.hpp

#ifndef __YUFC_UTIL_HPP__
#define __YUFC_UTIL_HPP__

#include "logger.hpp"
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <vector>
#include <zlib.h>

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
            LOG(ERROR) << "get file size failed!" << " filename: " << __file_name << std::endl;
            return -1;
        }
        return st.st_size;
    }
    // 获取文件最后一次修改时间
    time_t last_modify_time() {
        struct stat st;
        if (stat(__file_name.c_str(), &st) < 0) {
            LOG(ERROR) << "get file last modify time failed!" << " filename: " << __file_name << std::endl;
            return -1;
        }
        return st.st_mtime;
    }
    // 获取文件最后一次被访问时间
    time_t last_access_time() {
        struct stat st;
        if (stat(__file_name.c_str(), &st) < 0) {
            LOG(ERROR) << "get file last access time failed!" << " filename: " << __file_name << std::endl;
            return -1;
        }
        return st.st_atime;
    }
    // 获取文件路径中的文件名
    // ./abc/test.txt -> test.txt
    std::string file_name() {
        // size_t pos = __file_name.find_last_of("/");
        // if (pos == std::string::npos)
        //     return __file_name;
        // return __file_name.substr(pos + 1);
        namespace fs = std::experimental::filesystem;
        return fs::path(__file_name).filename().string();
    }
    // 获取文件指定位置内容
    bool access_pos_content(std::string* body, size_t pos, size_t len) {
        std::ifstream ifs;
        ifs.open(__file_name, std::ios::binary); // 读取
        if (ifs.is_open() == false) {
            LOG(ERROR) << "open file failed" << " filename: " << __file_name << std::endl;
            return false;
        }
        // 获取文件数据
        size_t fsize = this->file_size();
        // 如果 pos + len 太长 则读取错误
        if (pos + len > fsize) {
            LOG(ERORR) << "pos error, file overflow" << " filename: " << __file_name << std::endl;
            return false;
        }
        ifs.seekg(pos, std::ios::beg);
        body->resize(len);
        ifs.read(&(*body)[0], len);
        if (!ifs.good()) {
            LOG(ERROR) << "read file error" << " filename: " << __file_name << std::endl;
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
            LOG(ERROR) << "open file failed" << " filename: " << __file_name << std::endl;
            return false;
        }
        ofs.write(&body[0], body.size());
        if (ofs.good() == false) {
            LOG(ERROR) << "write to file failed" << " filename: " << __file_name << std::endl;
            ofs.close();
            return false;
        }
        ofs.close();
        return true;
    }
    // 压缩文件
    bool pack(const std::string& pack_name) {
        // 1. 读文件
        std::string body;
        if (!this->access_content(&body)) {
            LOG(ERROR) << "get content failed" << " filename: " << __file_name << std::endl;
            return false;
        }
        // 2. 压缩数据
        auto packed = this->__pack(body);
        // 3. 存储到新文件中
        FileUtil fu(pack_name);
        if (fu.set_content(packed) == false) {
            LOG(ERROR) << "write packed content to file failed" << " filename: " << __file_name << std::endl;
            return false;
        }
        return true;
    }
    // 解压缩文件
    bool unpack(const std::string& filename) {
        std::string body;
        if (!this->access_content(&body)) {
            LOG(ERROR) << "get content failed" << " filename: " << __file_name << std::endl;
            return false;
        }
        auto unpacked = this->__unpack(body);
        FileUtil fu(filename);
        if (fu.set_content(unpacked) == false) {
            LOG(ERROR) << "write unpacked content to file failed" << " filename: " << __file_name << std::endl;
            return false;
        }
        return true;
    }
    // 判断文件是否存在
    bool exists() {
        namespace fs = std::experimental::filesystem;
        return fs::exists(__file_name);
    }
    // 创建目录
    bool create_dir() {
        namespace fs = std::experimental::filesystem;
        if (this->exists())
            return true;
        return fs::create_directories(__file_name);
    }
    // 遍历目录
    bool scan_dir(std::vector<std::string>* arry) {
        namespace fs = std::experimental::filesystem;
        for (auto& p : fs::directory_iterator(__file_name)) {
            if (fs::is_directory(p) == true)
                continue;
            // relative_path 是带有路径的文件名
            arry->push_back(fs::path(p).relative_path().string());
        }
        return true;
    } //
    bool remove() {
        // 删除这个文件
        if (this->exists() == false)
            return true;
        ::remove(__file_name.c_str());
        return true;
    }

private:
    std::string __pack(const std::string& str, int compressionLevel = Z_DEFAULT_COMPRESSION) {
        if (str.empty()) {
            return "";
        }
        // 估算压缩后数据的最大大小
        uLongf compressedSize = compressBound(str.size());
        std::vector<Bytef> compressedData(compressedSize);
        // 执行压缩
        int res = compress2(compressedData.data(), &compressedSize, reinterpret_cast<const Bytef*>(str.data()), str.size(), compressionLevel);
        if (res != Z_OK) {
            throw std::runtime_error("pack error, errno code: " + std::to_string(res));
        }
        // 将压缩数据转换为字符串
        return std::string(reinterpret_cast<char*>(compressedData.data()), compressedSize);
    }
    std::string __unpack(const std::string& str) {
        if (str.empty()) {
            return "";
        }
        // 设定一个初始的解压缓冲区大小（可以根据需要调整）
        uLongf decompressedSize = str.size() * 4; // 初始估计值
        std::vector<char> decompressedData(decompressedSize);
        int res = uncompress(reinterpret_cast<Bytef*>(decompressedData.data()), &decompressedSize, reinterpret_cast<const Bytef*>(str.data()), str.size());
        // 如果缓冲区不够大，循环增加缓冲区大小
        while (res == Z_BUF_ERROR) {
            decompressedSize *= 2;
            decompressedData.resize(decompressedSize);
            res = uncompress(reinterpret_cast<Bytef*>(decompressedData.data()), &decompressedSize, reinterpret_cast<const Bytef*>(str.data()), str.size());
        }
        if (res != Z_OK) {
            throw std::runtime_error("pack error, errno code: " + std::to_string(res));
        }
        // 将解压数据转换为字符串
        return std::string(decompressedData.data(), decompressedSize);
    }
};

class JsonUtil {
public:
    static bool serialize(const Json::Value& root, std::string* str) {
        Json::StreamWriterBuilder swb;
        std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());
        std::stringstream ss;
        if (sw->write(root, &ss) != 0) {
            LOG(ERROR) << "json write failed!" << std::endl;
            return false;
        }
        *str = ss.str();
        return true;
    }
    static bool unserialize(const std::string& str, Json::Value* root) {
        Json::CharReaderBuilder crb;
        std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
        std::string err;
        bool ret = cr->parse(str.c_str(), str.c_str() + str.size(), root, &err);
        if (ret == false) {
            LOG(ERROR) << "json parse error: " << err << std::endl;
            return false;
        }
        return true;
    }
};

} // namespace nimbus
#endif