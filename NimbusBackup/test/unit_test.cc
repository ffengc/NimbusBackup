/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// for unit test

// test FileUtil
#include "../server/data_manager.hpp"
#include "../server/hot.hpp"
#include "../tools/config.hpp"
#include "../tools/util.hpp"
#include <assert.h>

void FileUtilTest(const std::string& filename) {
    // 1.
    // nimbus::FileUtil fu(filename);
    // nimbus::LOG(DEBUG) << "begin to test FileUtil" << std::endl;
    // std::cout << fu.file_size() << std::endl;
    // std::cout << fu.last_modify_time() << std::endl;
    // std::cout << fu.last_access_time() << std::endl;
    // std::cout << fu.file_name() << std::endl;
    // 2.
    // nimbus::FileUtil fu(filename);
    // fu.pack(filename + ".lz");
    // nimbus::FileUtil pfu(filename + ".lz");
    // pfu.unpack("hello.txt");
    // 3.
    nimbus::FileUtil fu(filename);
    fu.create_dir();
    std::vector<std::string> arry;
    fu.scan_dir(&arry);
    for (auto& a : arry)
        std::cout << a << std::endl;
    return;
}

void JsonUtilTest() {
    nimbus::LOG(DEBUG) << "begin to test JsonUtil" << std::endl;
    const char* name = "ffengc";
    int age = 20;
    float score[] = { 100, 99, 97 };
    Json::Value root;
    root["name"] = name;
    root["age"] = age;
    root["score"].append(score[0]);
    root["score"].append(score[1]);
    root["score"].append(score[2]);
    // 序列化测试
    std::string json_str;
    nimbus::JsonUtil::serialize(root, &json_str);
    std::cout << json_str << std::endl;

    // 反序列化测试
    Json::Value val;
    nimbus::JsonUtil::unserialize(json_str, &val);
    std::cout << "name: " << val["name"].asString() << std::endl;
    std::cout << "age: " << val["age"].asInt() << std::endl;
    for (int i = 0; i < val["score"].size(); i++)
        std::cout << val["score"][i].asFloat() << std::endl;
}

void ConfigTest() {
    nimbus::Config* conf = nimbus::Config::get_instance();
    std::cout << conf->hot_time() << std::endl;
    std::cout << conf->server_ip() << std::endl;
    std::cout << conf->server_port() << std::endl;
    std::cout << conf->download_prefix() << std::endl;
    std::cout << conf->packfile_suffix() << std::endl;
    std::cout << conf->pack_dir() << std::endl;
    std::cout << conf->back_dir() << std::endl;
    std::cout << conf->backup_file() << std::endl;
}

void DataTest(const std::string& file_name) {
    nimbus::__backup_info info;
    if (!info.new_backup_info(file_name))
        throw std::runtime_error("file not exists");
    nimbus::DataManager data;
    // 插入一个
    data.insert(info);

    // 获取一个
    nimbus::__backup_info tmp;
    data.access_info_by_url("/download/bundle.h", &tmp);
    std::cout << tmp.pack_flag__ << std::endl;
    std::cout << tmp.file_size__ << std::endl;
    std::cout << tmp.last_modify_time__ << std::endl;
    std::cout << tmp.last_access_time__ << std::endl;
    std::cout << tmp.real_path__ << std::endl;
    std::cout << tmp.pack_path__ << std::endl;
    std::cout << tmp.url__ << std::endl;
    std::cout << "---------" << std::endl;

    // 修改
    info.pack_flag__ = true; // 改一个字段
    data.update(info);

    // 获取所有
    std::vector<nimbus::__backup_info> arry;
    data.access_all(&arry);
    assert(arry.size() == 1); // 应该只有一个
    tmp = arry[0];
    std::cout << tmp.pack_flag__ << std::endl;
    std::cout << tmp.file_size__ << std::endl;
    std::cout << tmp.last_modify_time__ << std::endl;
    std::cout << tmp.last_access_time__ << std::endl;
    std::cout << tmp.real_path__ << std::endl;
    std::cout << tmp.pack_path__ << std::endl;
    std::cout << tmp.url__ << std::endl;
    std::cout << "---------" << std::endl;

    // 测试根据实际信息查询
    data.access_info_by_realpath(file_name, &tmp);
    std::cout << tmp.pack_flag__ << std::endl;
    std::cout << tmp.file_size__ << std::endl;
    std::cout << tmp.last_modify_time__ << std::endl;
    std::cout << tmp.last_access_time__ << std::endl;
    std::cout << tmp.real_path__ << std::endl;
    std::cout << tmp.pack_path__ << std::endl;
    std::cout << tmp.url__ << std::endl;
    std::cout << "---------" << std::endl;
    // 期望, 三次打印的东西都应该是相同的
    // 输出
    /*
        [INFO][../ server /../ tools / config.hpp][20] load config :..
        / nimbus.conf... 0 29524 1736959881 1737093160
              .
        / bundle.h
              .
        / packdir / bundle.h.lz
        / download / bundle.h-- -- -- -- -1 29524 1736959881 1737093160 ./ bundle.h./ packdir / bundle.h.lz
        / download / bundle.h-- -- -- -- -1 29524 1736959881 1737093160 ./ bundle.h./ packdir / bundle.h.lz
        / download / bundle.h-- -- -- -- -
     */
    // 测试通过

    // data.storage();
}

void DataTestLoad() {
    /**
     * 测试使用持久化备份文件加载信息
    */
    nimbus::DataManager data;
    nimbus::__backup_info tmp;
    // 获取所有
    std::vector<nimbus::__backup_info> arry;
    data.access_all(&arry);
    assert(arry.size() == 1); // 应该只有一个
    tmp = arry[0];
    std::cout << tmp.pack_flag__ << std::endl;
    std::cout << tmp.file_size__ << std::endl;
    std::cout << tmp.last_modify_time__ << std::endl;
    std::cout << tmp.last_access_time__ << std::endl;
    std::cout << tmp.real_path__ << std::endl;
    std::cout << tmp.pack_path__ << std::endl;
    std::cout << tmp.url__ << std::endl;
    std::cout << "---------" << std::endl;
}


nimbus::DataManager *__data;
void HostTest() {
    __data = new nimbus::DataManager();
    nimbus::HotManager hot;
    hot.Run();
}

int main(int argc, char** argv) {
    // FileUtilTest("../test/tt");
    // ConfigTest();
    // DataTest("./bundle.h");
    // DataTestLoad();
    HostTest();
    return 0;
}