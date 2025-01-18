/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// hot.hpp

#ifndef __YUFC_HOT_HPP__
#define __YUFC_HOT_HPP__

#include "data_manager.hpp"
#include <unistd.h>

extern nimbus::DataManager* __data;
namespace nimbus {
class HotManager {
private:
    std::string __back_dir; // 备份文件路径
    std::string __pack_dir; // 压缩文件路径
    std::string __pack_suffix; // 压缩包后缀名
    int __hot_time; // 热点判断时间
private:
    // 判断是否为非热点文件, 如果是非热点文件, 则需要压缩, 这里用于判断
    bool __hot_judge(const std::string& filename) {
        FileUtil fu(filename);
        time_t last_access_time = fu.last_access_time();
        time_t cur_time = time(NULL);
        if (cur_time - last_access_time > __hot_time) // 非热点文件
            return true;
        return false;
    } //
public:
    HotManager() {
        Config* conf = Config::get_instance();
        __back_dir = conf->back_dir();
        __pack_dir = conf->pack_dir();
        __pack_suffix = conf->packfile_suffix();
        __hot_time = conf->hot_time();
        // __pack_dir 和 __back_dir 如果不存在, 则创建它
        FileUtil tmp1(__back_dir);
        FileUtil tmp2(__pack_dir);
        tmp1.create_dir();
        tmp2.create_dir();
    }
    bool Run() {
        while (1) {
            // 1. 遍历备份目录, 获取所有文件名
            FileUtil fu(__back_dir);
            std::vector<std::string> arr;
            fu.scan_dir(&arr);
            // 2. 遍历判断文件是否是非热点文件
            // BUG LOG功能有问题
            // LOG(DEBUG) << "there are " << std::to_string(arr.size()) << " files in the back dir: " << __back_dir << std::endl;
            for (const auto& e : arr) {
                if (__hot_judge(e) == false) // 热点文件, 不需要压缩
                    continue;
                // 获取文件的备份信息
                __backup_info bi;
                if (!__data->access_info_by_realpath(e, &bi)) {
                    // false 表示, 目录下有一个文件, 但是却找不到文件的信息
                    // 这种情况表示前面文件信息的管理有缺漏
                    bi.new_backup_info(e); // 补上这个信息
                }
                // 3. 对非热点文件进行压缩处理
                LOG(DEBUG) << "add un-hot file to " << __pack_dir
                           << ", remove from " << __back_dir << std::endl;
                FileUtil tmp(e);
                tmp.pack(bi.pack_path__);
                tmp.remove();
                // 4. 删除源文件, 修改备份信息
                bi.pack_flag__ = true;
                __data->update(bi);
            }
            usleep(1000); // 避免空目录循环, 消耗cpu资源
        }
        return true;
    }
};
} // namespace nimbus

#endif