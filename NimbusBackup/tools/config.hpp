/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// config.hpp

#ifndef __YUFC_CONFIG_HPP__
#define __YUFC_CONFIG_HPP__

#include "util.hpp"
#include <mutex>

namespace nimbus {
class Config {
#define CONFIG_FILE "../nimbus.conf" //
private:
    Config(const std::string& config_file = CONFIG_FILE) {
        LOG(INFO) << "load config: " << config_file << "..." << std::endl;
        __read_config_file(config_file);
    }
    static Config* __instance; // 单例指针
    static std::mutex __mutex; // 锁
private:
    // same with config
    int __hot_time;
    int __server_port;
    std::string __server_ip;
    std::string __download_prefix;
    std::string __packfile_suffix;
    std::string __pack_dir;
    std::string __back_dir;
    std::string __backup_file; //
private:
    bool __read_config_file(const std::string& config_file) {
        FileUtil fu(config_file);
        std::string body;
        if (!fu.access_content(&body)) {
            // 读取conf文件内容
            LOG(ERROR) << "load config file failed!" << std::endl;
            return false;
        }
        Json::Value root;
        if (!JsonUtil::unserialize(body, &root)) {
            LOG(ERROR) << "parse config file failed!" << std::endl;
            return false;
        }
        __hot_time = root["hot_time"].asInt();
        __server_port = root["server_port"].asInt();
        __server_ip = root["server_ip"].asString();
        __download_prefix = root["download_prefix"].asString();
        __packfile_suffix = root["packfile_suffix"].asString();
        __pack_dir = root["pack_dir"].asString();
        __back_dir = root["back_dir"].asString();
        __backup_file = root["backup_file"].asString();
        return true;
    } //
public:
    static Config* get_instance(const std::string& config_file = CONFIG_FILE) {
        if (__instance == nullptr) {
            __mutex.lock();
            if (__instance == nullptr)
                __instance = new Config(config_file);
            __mutex.unlock();
        }
        return __instance;
    }
    int hot_time() { return this->__hot_time; }
    int server_port() { return this->__server_port; }
    std::string server_ip() { return this->__server_ip; }
    std::string download_prefix() { return this->__download_prefix; }
    std::string packfile_suffix() { return this->__packfile_suffix; }
    std::string pack_dir() { return this->__pack_dir; }
    std::string back_dir() { return this->__back_dir; }
    std::string backup_file() { return this->__backup_file; }
};
Config* Config::__instance = nullptr;
std::mutex Config::__mutex;
} // namespace nimbus
#endif