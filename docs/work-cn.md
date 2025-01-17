# 系统搭建详细过程

## 客户端和服务端模块划分

**客户端**
- 数据管理模块：负责客户端备份的文件信息管理，通过这些数据可以确定一个文件是否需要备份 (新增/已备份但被修改)。
- 文件检测模块：遍历获取指定文件夹中所有文件路径名称。
- 网络通信模块：搭建网络通信客户端，实现将文件数据备份上传到服务器

**服务端**
- 数据管理模块：负责服务器上备份文件的信息管理。
- 网络通信模块：搭建网络通信服务器，实现与客户端通信。
- 业务处理模块：针对客户端的各个请求进行对应业务处理并响应结果。
- 热点管理模块：负责文件的热点判断，以及非热点文件的压缩存储

## 环境搭建

**升级gcc到7.3版本**
> 我使用的gcc版本: `gcc version 11.4.0 (Ubuntu 11.4.0-1ubuntu1~22.04)`

**安装 `jsoncpp`**

> 略

> [!TIP]
> 对于数据压缩部分, 我尝试在arm架构Ubuntu上使用bundle库, 但结果是无法运行, 因为指令集不同。\
> 因此在arm架构Ubuntu上, 我是用zlib进行数据压缩。

**下载 `bundle` 数据压缩库 (x86_64架构)**

> github下载即可, 只需要用头文件 \
> [https://github.com/r-lyeh-archived/bundle](https://github.com/r-lyeh-archived/bundle)

**下载 `zlib` 库 (arm架构)**


**下载 `httplib` 库**

> github下载即可, 只需要用头文件 \
> [https://github.com/yhirose/cpp-httplib](https://github.com/yhirose/cpp-httplib)

## 数据压缩

### 使用bundle (x86_64架构)

BundleBundle 是一个嵌入式压缩库，支持23种压缩算法和2种存档格式。使用的时候只需要加入两个文件 `bundle.h` 和 `bundle.cpp` 即可。

- 使用的详细文档: [https://github.com/r-lyeh-archived/bundle/blob/master/README.md](https://github.com/r-lyeh-archived/bundle/blob/master/README.md)

使用示例代码如 [test_bundle.cc](../NimbusBackup/test/test_bundle.cc) 所示。

```cpp
#include "bundle.h"
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    // argv[1]: 原始文件路径名称
    // argv[2]: 压缩包名称
    if (argc < 3)
        return -1;
    std::string ifilename = argv[1];
    std::string ofilename = argv[2];

    // open the file and read the file
    std::ifstream ifs;
    ifs.open(ifilename, std::ios::binary);
    ifs.seekg(0, std::ios::end);
    size_t fsize = ifs.tellg(); // size of the file
    ifs.seekg(0, std::ios::beg);
    std::string body;
    body.resize(fsize);
    ifs.read(&body[0], fsize); // read the file

    // compress the file
    std::string packed = bundle::pack(bundle::LZIP, body);

    // make output file
    std::ofstream ofs;
    ofs.open(ofilename, std::ios::binary);
    ofs.write(&packed[0], packed.size());

    ifs.close();
    ofs.close();
    return 0;
}
```

### 使用zlib (arm架构)

使用示例代码如 [test_zlib.cc](../NimbusBackup/test/test_zlib.cc) 所示。

```cpp
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <zlib.h>

std::string compressString(const std::string& str, int compressionLevel = Z_DEFAULT_COMPRESSION) {
    if (str.empty()) {
        return "";
    }
    // 估算压缩后数据的最大大小
    uLongf compressedSize = compressBound(str.size());
    std::vector<Bytef> compressedData(compressedSize);
    // 执行压缩
    int res = compress2(compressedData.data(), &compressedSize, reinterpret_cast<const Bytef*>(str.data()), str.size(), compressionLevel);
    if (res != Z_OK) {
        throw std::runtime_error("压缩失败，错误码: " + std::to_string(res));
    }
    // 将压缩数据转换为字符串
    return std::string(reinterpret_cast<char*>(compressedData.data()), compressedSize);
}

// 解压缩字符串
std::string decompressString(const std::string& str) {
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
        throw std::runtime_error("解压缩失败，错误码: " + std::to_string(res));
    }
    // 将解压数据转换为字符串
    return std::string(decompressedData.data(), decompressedSize);
}

int main() {
    std::ifstream ifs;
    ifs.open("./bundle.cpp", std::ios::binary);
    ifs.seekg(0, std::ios::end);
    size_t fsize = ifs.tellg(); // size of the file
    ifs.seekg(0, std::ios::beg);
    std::string body;
    body.resize(fsize);
    ifs.read(&body[0], fsize); // read the file

    // compress the file
    auto packed = compressString(body);
    std::cout << "before packed: " << body.size() << std::endl;
    std::cout << "after packed: " << packed.size() << std::endl;

    auto unpacked = decompressString(packed);
    std::cout << "unpacked: " << unpacked.size() << std::endl;
    
    // output file
    std::ofstream ofs;
    ofs.open("unpacked_bundle.cpp", std::ios::binary);
    ofs.write(&unpacked[0], unpacked.size());
    ofs.close();
    ifs.close();
    return 0;
}
```

输出如下所示:

```bash
(base) ffengc@ubuntu-linux-22-04-desktop:~/Project/NimbusBackup/NimbusBackup/test$ ./test 
before packed: 5571471
after packed: 1052582
unpacked: 5571471
(base) ffengc@ubuntu-linux-22-04-desktop:~/Project/NimbusBackup/NimbusBackup/test$ 
```

### 在Linux或MacOS上使用md5工具判断两个文件是否完全相同

*Windows略*

输入以下命令计算文件的 MD5 值：

```sh
md5sum /path/file.txt # Linux
md5 /path/file.txt # macOS
```

判断两个文件的md5值是否一致即可。

当然, 也可以编写脚本批量判断。


## `httplib` 库认识

因为我在之前的项目中已经熟练使用这个库了, 因此部分略。


## 文件工具类设计

需要实现的功能: 
- 获取文件的大小
- 获取文件最后一次修改时间
- 获取文件最后一次访问时间
- 获取文件路径名中文件名称 (`/test/a.txt` -> `a.txt`)
- 向文件中写入数据
- 读取文件中的数据
- 获取文件指定位置, 指定长度的数据
- 遍历目录
- 判断文件是否存在
- 创建一个目录
- 压缩文件
- 解压缩文件

大部分接口都是非常简单了, 我这里只展示部分需要注意的接口。

### 目录相关操作

借助C++17的 [FileSystem](https://en.cppreference.com/w/cpp/experimental/fs)

[NimbusBackup/tools/util.hpp](NimbusBackup/tools/util.hpp)
```cpp
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
```

## Json工具类实现

- 序列化
- 反序列化

两个接口即可。

[NimbusBackup/tools/util.hpp](NimbusBackup/tools/util.hpp)
```cpp
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
```

## 用配置文件来配置系统信息

需要用配置文件来管理的信息:
- 热点判断时间
- 文件下载的url前缀路径, 比如 `/wwwroot`
  - 当用户发来一个备份列表查看请求, `show`, 我们要如何判断这是不是一个 `show` 的文件下载请求
  - 所以我们可以设置 `/download/show`, 来表示下载, 加上下载的前缀 `download`
- 服务端压缩包后缀名
- 上传文件的服务端存放路径
- 压缩包在服务端的存放路径
- 服务端备份信息的存放文件 (按照道理来说应该是用数据库, 但是本项目直接使用了文件)
- 服务器监听IP地址
- 服务器的监听端口

另外, 这个配置文件管理的模块, 应该要设计成单例。

[NimbusBackup/tools/config.hpp](NimbusBackup/tools/config.hpp)
```cpp
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
```

## 服务端实现

### 数据管理模块

数据管理模块其实就是用来标记这些数据信息的模块。

对于一个文件, 需要管理这些信息:
- 文件的实际存储路径: 当客户端啊哟下载文件的时候, 从这文件中读取数据并进行相应
- 文件压缩包存放路径名: 如果这个文件是一个非热点文件被压缩了, 则这个就是压缩包路径的名称
  - 如果客户端要下载文件, 则需要先解压缩, 然后读取解压后的文件数据
- 文件是否被压缩的标志
- 文件大小
- 文件最后一次修改时间
- 文件最后一次访问时间
- 文件访问url: `/download/a.txt`

如何管理呢? 首先要便于访问, 也要持久化。
- 使用hash表在内存中管理数据, 以url到path作为key值 -- 查询速度快
- 持久化存储管理: 使用json序列化将所有数据信息保存在文件中

[data_manager.hpp](./NimbusBackup/server/data_manager.hpp)
```cpp
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
        // ...
    }
} __backup_info;
```

接下来是需要完成数据管理类了。

[data_manager.hpp](./NimbusBackup/server/data_manager.hpp)
```cpp
class DataManager {
private:
    std::string __bak_info;
    pthread_rwlock_t __rwlock; // 读共享, 写互斥
    std::unordered_map<std::string, __backup_info> __table; //
public:
    DataManager();
    bool insert(const __backup_info& info); // 插入一条数据
    bool update(const __backup_info& info); // 更新数据
    bool access_info_by_url(const std::string& url,  __backup_info* info); // 通过访问url来获得这个数据
    bool access_info_by_realpath(const std::string& realpath, __backup_info* info); // 通过服务端绝对路径来获得这个数据
    bool access_all(std::vector<__backup_info>* arry); // 获取所有数据
    bool storage(); // 数据的持久化存储
    bool load(); // 加载持久化文件中的数据
};
```