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

- **[简体中文](./docs/README-cn.md)**
- **[English](./README.md)**

> [!TIP]
> If you have any questions about the operation and implementation of this project, or if you have better optimization suggestions for this project, you can contact me directly or leave an issue in the repository.

## Expected functions

Automatic cloud backup, automatically upload the files that need to be backed up in the specified directory on the computer to the server, and can be viewed and downloaded at any time through the browser. The download process on the web page supports breakpoint continuation, and the server will also manage the uploaded files for hot spots, compress and store non-hot files to save disk space

**Server program is responsible for functions**
- Backup and store files uploaded by the client
- Can manage hot files for files, compress and store non-hot files to save disk space
- Support client browsers to view access file lists
- Support client browsers to download files, and download supports breakpoint continuation

**Client program is responsible for functions**
- Can automatically detect files in the folder specified by the client and determine whether they need to be backed up
- Upload files that need to be backed up to the server one by one