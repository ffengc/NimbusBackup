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

- **[简体中文](./docs/README-cn.md)**
- **[English](./README.md)**

> [!TIP]
> 如果你对本项目的运行和实现有任何疑问，或者对本项目有更好的优化建议，可以直接联系我，或者在仓库中留言。

## 期望实现的功能

自动云备份, 自动将计算机上指定目录中需要备份的文件上传备份到服务器中, 并且能够随时通过浏览器进行查看并且下载, 在网页进行下载的过程支持断点续传, 而服务器中也会对已上传的文件进行热点管理, 将非热点的文件进行压缩存储, 节省磁盘空间

**服务端程序负责功能**
- 针对客户端上传的文件进行备份存储
- 能够对文件进行热点文件管理，对非热点文件进行压缩存储，节省磁盘空间
- 支持客户端浏览器查看访问文件列表
- 支持客户端浏览器下载文件，并且下载支持断点续传

**客户端程序负责功能**
- 能够自动检测客户机指定文件夹中的文件，并判断是否需要备份
- 将需要备份的文件逐个上传到服务器