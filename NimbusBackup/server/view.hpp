/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// view.hpp

#include "data_manager.hpp"
#include <ctemplate/template.h>
#include <unordered_map>
#define TEMPLATE_HTML_NAME "./wwwroot/list.tpl"
using namespace nimbus;

const std::string icons_root_path = "icons/gif/16x16/";
const std::unordered_map<std::string, std::string> filetype2iconsMap = {
    // Config type
    { ".conf", icons_root_path + "Application.gif" },
    { ".config", icons_root_path + "Application.gif" },
    // yaml file
    { ".yaml", icons_root_path + "Application.gif" },
    // tag file
    { ".tag", icons_root_path + "Blue tag.gif" },
    // msg file
    { ".msg", icons_root_path + "Bubble.gif" },
    // CD file
    { ".CD", icons_root_path + "CD.gif" },
    { ".cd", icons_root_path + "CD.gif" },
    // video file
    { ".mp4", icons_root_path + "Film.gif" },
    { ".avi", icons_root_path + "Film.gif" },
    { ".mov", icons_root_path + "Film.gif" },
    { ".mkv", icons_root_path + "Film.gif" },
    { ".flv", icons_root_path + "Film.gif" },
    // script file
    { ".sh", icons_root_path + "Script.gif" },
    { ".bash", icons_root_path + "Script.gif" },
    // other Text.gif
};

static void lower(std::string& str) {
    for (auto& ch : str) {
        if (std::isalpha(static_cast<unsigned char>(ch))) {
            ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        }
    }
}

static std::string filename2icon(const std::string& filename) {
    std::string icon_path;
    std::string suffix;
    // 拿到文件的后缀
    size_t pos = filename.find_last_of(".");
    if (pos == std::string::npos) {
        // 文件没有后缀
        std::string tmp = filename;
        lower(tmp);
        if (tmp == "makefile")
            suffix = ".sh";
        if (tmp == "license")
            suffix = ".msg";
    } else {
        suffix = filename.substr(pos); // 后缀
    }
    lower(suffix);
    auto it = filetype2iconsMap.find(suffix);
    if (it == filetype2iconsMap.end())
        icon_path = icons_root_path + "Text.gif";
    else
        icon_path = it->second;
    return icon_path;
}

static std::string time2str(time_t t) {
    // 把时间戳转化为字符串
    return std::ctime(&t);
}

static std::string generate_html(const std::vector<__backup_info>& arry) {
    // 利用 ctemplate 库将模版渲染html
    ctemplate::TemplateDictionary dict("list");
    // std::cerr << arry.size() << std::endl;
    for (const auto& a : arry) {
        ctemplate::TemplateDictionary* subDict = dict.AddSectionDictionary("FILE_LIST");
        subDict->SetValue("ICON_PATH", filename2icon(FileUtil(a.real_path__).file_name()));
        subDict->SetValue("FILE_NAME", FileUtil(a.real_path__).file_name());
        subDict->SetValue("DOWNLOAD_LINK", a.url__);
        subDict->SetValue("LAST_MODIFIED", time2str(a.last_modify_time__));
        subDict->SetValue("FILE_SIZE", std::to_string(a.file_size__ / 1024) + "K");
    }
    // 加载模板文件
    // 可以通过设置环境变量 TEMPLATE_ROOTDIR 或者 ctemplate::Template::SetBaseDirectory() 指定模板目录
    ctemplate::Template* tpl = ctemplate::Template::GetTemplate(TEMPLATE_HTML_NAME, ctemplate::DO_NOT_STRIP);
    if (!tpl) {
        std::string msg = "fail to load ";
        msg += TEMPLATE_HTML_NAME;
        LOG(FATAL) << msg << std::endl;
        throw std::runtime_error(msg);
    }
    std::string output;
    tpl->Expand(&output, &dict);
    return output;
}