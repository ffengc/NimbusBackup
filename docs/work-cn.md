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