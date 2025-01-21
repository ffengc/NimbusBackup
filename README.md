# NimbusBackup

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

**English** | [中文](./docs/README-CN.md)

> [!TIP]
> If you have any questions about the operation and implementation of this project, or if you have better optimization suggestions for this project, you can contact me directly or leave an issue in the repository.

*Note: The project is still under development and optimization*

## System Function

Automatic cloud backup, automatically upload the files that need to be backed up in the specified directory on the computer to the server, and can be viewed and downloaded at any time through the browser. The download process on the web page supports breakpoint resumption, and the server will also manage the hot spots of the uploaded files, compress and store non-hot files (long-term no access) to save disk space.

**Technology stack:** http client/server construction, jsoncpp, file compression (bundle and zlib), hotspot management, breakpoint resume, thread control and thread pool, read-write lock, singleton mode

- [x] The client performs cloud backup of files in the specified directory
- [x] Supports browser viewing and downloading files
- [x] Web page download supports breakpoint resume
- [x] Hotspot management of server-side files
- [ ] Optimize client and browser access interface
- [ ] Multi-platform client development (MacOS/Linux/Windows)
- [ ] Access interface similar to Alibaba Cloud Disk, supports active upload
- [ ] Implement user management

I will continue to optimize the unimplemented functions based on the code of this repository, so it is not currently released publicly, and the project operation effect pictures/videos are not displayed for the time being.


## How to use

*To be improved*

## Detailed process of system development and construction

- [work.md](./work-cn.md) (Chinese Only)