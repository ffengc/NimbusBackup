/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// view.hpp

#include "data_manager.hpp"
#include <ctemplate/template.h>
#define TEMPLATE_HTML_NAME "./wwwroot/list.tpl"
using namespace nimbus;

std::string time2str(time_t t) {
    // 把时间戳转化为字符串
    return std::ctime(&t);
}

static std::string generate_html(const std::vector<__backup_info>& arry) {
    // 利用 ctemplate 库将模版渲染html
    ctemplate::TemplateDictionary dict("list");
    std::cerr << arry.size() << std::endl;
    for (const auto& a : arry) {
        ctemplate::TemplateDictionary* subDict = dict.AddSectionDictionary("FILE_LIST");
        subDict->SetValue("ICON_PATH", "icons/gif/16x16/text.gif");
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
    std::cerr << "-------------------" << std::endl;
    std::cerr << output << std::endl;
    return output;
}