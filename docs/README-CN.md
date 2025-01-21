# 云备份系统

<a href="https://github.com/ffengc">
    <img src="https://img.shields.io/static/v1?label=Github&message=ffengc&color=blue" alt="ffengc.github">
</a>
<a href="https://ffengc.github.io">
    <img src="https://img.shields.io/static/v1?label=Page&message=ffengc.github.io&color=red" alt="ffengc.github.io">
</a>
<a href="https://ffengc.github.io/gh-blog/">
    <img src="https://img.shields.io/static/v1?label=Blog&message=Blog Page&color=brightgreen" alt="Mutable.ai Auto Wiki">
</a>

</div>

**中文** | [English](../README.md)

> [!TIP]
> 如果你对本项目的运行和实现有任何疑问，或者对本项目有更好的优化建议，可以直接联系我，或者在仓库中留言。

*备注: 项目目前仍在开发和优化中*

## 系统功能

自动云备份, 自动将计算机上指定目录中需要备份的文件上传备份到服务器中, 并且能够随时通过浏览器进行查看并且下载, 在网页进行下载的过程支持断点续传, 而服务器中也会对已上传的文件进行热点管理, 将非热点文件(长时间无访问)进行压缩存储, 节省磁盘空间。

**技术栈:** http客户端/服务器搭建, jsoncpp, 文件压缩(bundle和zlib), 热点管理, 断点续传, 线程控制和线程池, 读写锁, 单例模式

- [x] 客户端对指定的目录中文件进行云备份
- [x] 支持浏览器进行查看并下载文件
- [x] 网页下载支持断点续传
- [x] 服务端文件的热点管理
- [ ] 优化客户端, 浏览器访问界面
- [ ] 多平台客户端开发 (MacOS/Linux/Windows)
- [ ] 类似阿里云盘的访问界面, 支持主动上传
- [ ] 实现用户管理

未实现的功能我会在本仓库代码基础上继续进行优化, 因此目前暂时不公开release, 暂时不展示项目运行效果图/视频。

## 如何使用

*待完善*

## 系统开发搭建详细过程

- [work.md](./work-cn.md)