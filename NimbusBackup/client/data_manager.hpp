/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// data_manager.hpp

#ifndef __YUFC_DATA_MANAGER_CLIENT_HPP__
#define __YUFC_DATA_MANAGER_CLIENT_HPP__

#include "../tools/util.hpp"
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace nimbus {
class DataManager {
private:
    std::string __backup_file; // 备份信息的持久化存储文件
    std::unordered_map<std::string, std::string> __table; //
public:
    DataManager(const std::string& backup_file)
        : __backup_file(backup_file) {
        init_load(); // 加载
    }
    bool storage() {
        // 1. 获取所有备份信息
        std::stringstream ss;
        auto it = __table.begin();
        for (; it != __table.end(); ++it)
            // 2. 将所有信息进行指定持久化格式的组织
            ss << it->first << " " << it->second << "\n"; // 简化序列化过程, 方便跨平台
        // 3. 持久化存储
        FileUtil fu(__backup_file);
        fu.set_content(ss.str());
        return true;
    }
    bool init_load() {
        // 1. 从文件中读取所有数据
        FileUtil fu(__backup_file);
        std::string body;
        fu.access_content(&body);
        // 2. 进行数据解析, 添加到表中
        std::vector<std::string> arry;
        __split(body, "\n", &arry);
        for (const auto& e : arry) {
            std::vector<std::string> tmp;
            __split(e, " ", &tmp);
            if (tmp.size() != 2) {
                LOG(WARNING) << "maybe something wrong in the data" << std::endl;
                continue;
            }
            __table[tmp[0]] = tmp[1];
        }
        return true;
    }
    bool insert(const std::string& key, const std::string& val) {
        // 单线程, 不需要lock
        __table[key] = val;
        storage();
        return true;
    }
    bool update(const std::string& key, const std::string& val) {
        __table[key] = val;
        storage();
        return true;
    }
    bool get(const std::string& key, std::string* val) {
        auto it = __table.find(key);
        if (it == __table.end())
            return false; // 没找到
        *val = it->second;
        return true;
    } //
private:
    // 字符串分割函数
    int __split(const std::string& str, const std::string& sep, std::vector<std::string>* arry) {
        /**
         * sep期望是一个长度为1的字符串
         */
        if (sep.size() != 1)
            throw std::runtime_error("sep expected to be a one char string");
        size_t pos, idx = 0;
        size_t cnt = 0;
        while (1) {
            // aaa bbb ccc
            pos = str.find(sep, idx);
            if (pos == std::string::npos)
                break;
            if (pos == idx) {
                idx = pos + sep.size();
                continue;
            }
            std::string tmp = str.substr(idx, pos - idx);
            arry->push_back(tmp);
            cnt++;
            idx = pos + sep.size();
        }
        if (idx < str.size()) {
            // 后面还有一串数据
            arry->push_back(str.substr(idx));
            cnt++;
        }
        return cnt;
    }
};

} // namespace nimbus

#endif