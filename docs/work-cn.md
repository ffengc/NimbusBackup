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

**下载 `bundle` 数据压缩库**

> github下载即可, 只需要用头文件 \
> [https://github.com/r-lyeh-archived/bundle](https://github.com/r-lyeh-archived/bundle)

**下载 `httplib` 库**

> github下载即可, 只需要用头文件 \
> [https://github.com/yhirose/cpp-httplib](https://github.com/yhirose/cpp-httplib)

## bundle压缩库简单认识

BundleBundle 是一个嵌入式压缩库，支持23种压缩算法和2种存档格式。使用的时候只需要加入两个文件 `bundle.h` 和 `bundle.cpp` 即可。

使用的详细文档: [https://github.com/r-lyeh-archived/bundle/blob/master/README.md](https://github.com/r-lyeh-archived/bundle/blob/master/README.md)