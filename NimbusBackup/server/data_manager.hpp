/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// data_manager.hpp

#ifndef __YUFC_DATA_MANAGER_HPP__
#define __YUFC_DATA_MANAGER_HPP__

#include "../tools/config.hpp"
#include "../tools/util.hpp"
#include <algorithm>
#include <pthread.h>
#include <unordered_map>

namespace nimbus {
// 数据信息的结构体
typedef struct __backup_info {
public:
    bool pack_flag__; // 是否被压缩
    size_t file_size__; // 文件大小
    time_t last_modify_time__; // 上次修改时间
    time_t last_access_time__; // 上次访问时间
    std::string real_path__; // 真实路径
    std::string pack_path__; // 被压缩后的路径
    std::string url__; // 文件访问url
public:
    // 新建一个 __backup_info
    bool new_backup_info(const std::string& real_path) {
        FileUtil fu(real_path);
        if (!fu.exists()) {
            // 如果文件不存在, 直接返回
            LOG(WARNING) << "new_backup_info: file not exists" << std::endl;
            return false;
        }
        // 获取两个存储路劲
        Config* conf = Config::get_instance();
        std::string packdir = conf->pack_dir();
        std::string pack_suffix = conf->packfile_suffix();
        std::string download_prefix = conf->download_prefix();

        this->pack_flag__ = false;
        this->file_size__ = fu.file_size();
        this->last_modify_time__ = fu.last_modify_time();
        this->last_access_time__ = fu.last_access_time();
        this->real_path__ = real_path;
        // ./backdir/a.txt -> ./packdir/a.txt.lz
        this->pack_path__ = packdir + fu.file_name() + pack_suffix;
        // ./backdir/a.txt -> /download/a.txt
        this->url__ = download_prefix + fu.file_name();
        return true;
    }
} __backup_info;

// 数据管理类
class DataManager {
private:
    std::string __bak_file;
    pthread_rwlock_t __rwlock; // 读共享, 写互斥
    std::unordered_map<std::string, __backup_info> __table; // url作为key
public:
    DataManager() {
        __bak_file = Config::get_instance()->backup_file();
        pthread_rwlock_init(&__rwlock, NULL); // 初始化读写锁
        load(); // 读取持久化信息
    }
    ~DataManager() { pthread_rwlock_destroy(&__rwlock); } // 销毁读写锁
    // 插入一条数据
    bool insert(const __backup_info& info) {
        // 需要写, 因此需要加上写锁
        pthread_rwlock_wrlock(&__rwlock); // 加锁
        __table[info.url__] = info;
        pthread_rwlock_unlock(&__rwlock);
        storage(); // 调用持久化存储
        return true;
    }
    // 更新
    bool update(const __backup_info& info) {
        // same to insert
        pthread_rwlock_wrlock(&__rwlock); // 加锁
        __table[info.url__] = info;
        pthread_rwlock_unlock(&__rwlock); // 解锁
        storage(); // 调用持久化存储
        return true;
    }
    // 通过访问url来获得这个数据
    bool access_info_by_url(const std::string& url, __backup_info* info) {
        pthread_rwlock_wrlock(&__rwlock); // 加锁
        auto it = __table.find(url);
        if (it == __table.end()) {
            pthread_rwlock_unlock(&__rwlock);
            return false;
        }
        *info = it->second;
        pthread_rwlock_unlock(&__rwlock);
        return true;
    }
    // 通过服务端绝对路径来获得这个数据
    bool access_info_by_realpath(const std::string& realpath, __backup_info* info) {
        pthread_rwlock_wrlock(&__rwlock); // 加锁
        auto it = __table.begin(); // realpath表示hashtable的key, 所以只能遍历查找
        for (; it != __table.end(); ++it) {
            if (it->second.real_path__ == realpath) {
                *info = it->second;
                pthread_rwlock_unlock(&__rwlock);
                return true;
            }
        }
        pthread_rwlock_unlock(&__rwlock);
        return false;
    }
    // 获取所有数据
    bool access_all(std::vector<__backup_info>* arry) {
        pthread_rwlock_wrlock(&__rwlock); // 加锁
        // std::vector<__backup_info> tmp;
        // 用 std::transform 把 __table 中的所有 value 转到 tmp 中
        std::transform(
            __table.begin(),
            __table.end(),
            std::back_inserter(*arry),
            [](const auto& p) {
                return p.second;
            });
        pthread_rwlock_unlock(&__rwlock);
        return true;
    }
    // 数据的持久化存储
    bool storage() {
        // 先获取所有数据
        std::vector<__backup_info> arry;
        this->access_all(&arry);
        // 组织成json
        Json::Value root;
        for (int i = 0; i < arry.size(); ++i) {
            Json::Value item;
            item["pack_flag"] = (Json::Value::Int64)arry[i].pack_flag__;
            item["file_size"] = arry[i].file_size__;
            item["last_modify_time"] = arry[i].last_modify_time__;
            item["last_access_time"] = arry[i].last_access_time__;
            item["real_path"] = arry[i].real_path__;
            item["pack_path"] = arry[i].pack_path__;
            item["url"] = arry[i].url__;
            root.append(item);
        }
        // 序列化json
        std::string body; // 序列化后的结果
        JsonUtil::serialize(root, &body);
        FileUtil fu(__bak_file);
        fu.set_content(body);
        return true;
    }
    // 加载持久化文件中的数据
    bool load() {
        // 1. 读取
        FileUtil fu(__bak_file);
        if (!fu.exists()) // 之前没有过持久化文件
            return true;
        std::string body;
        fu.access_content(&body);
        // 2. 反序列化
        Json::Value root;
        if (!JsonUtil::unserialize(body, &root)) {
            LOG(WARNING) << "unserialize backup file failed" << std::endl;
            return false;
        }
        // 3. 将反序列化后的数据加入table中
        for (int i = 0; i < root.size(); ++i) {
            __backup_info info;
            info.pack_flag__ = root[i]["pack_flag"].asBool();
            info.file_size__ = root[i]["file_size"].asInt64();
            info.last_modify_time__ = root[i]["last_modify_time"].asInt64();
            info.last_access_time__ = root[i]["last_access_time"].asInt64();
            info.real_path__ = root[i]["real_path"].asString();
            info.pack_path__ = root[i]["pack_path"].asString();
            info.url__ = root[i]["url"].asString();
            this->insert(info);
        }
        return true;
    }
};
} // namespace nimbus

#endif