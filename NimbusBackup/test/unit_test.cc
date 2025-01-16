/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// for unit test

// test FileUtil
#include "../tools/util.hpp"
void FileUtilTest(const std::string& filename) {
    // 1. 
    // nimbus::FileUtil fu(filename);
    // nimbus::LOG(DEBUG) << "begin to test FileUtil" << std::endl;
    // std::cout << fu.file_size() << std::endl;
    // std::cout << fu.last_modify_time() << std::endl;
    // std::cout << fu.last_access_time() << std::endl;
    // std::cout << fu.file_name() << std::endl;
    // 2.
    nimbus::FileUtil fu(filename);
    std::string body;
    fu.access_content(&body);
    nimbus::FileUtil nfu("./hello.txt");
    nfu.set_content(body);
    return;
}

int main(int argc, char** argv) {
    FileUtilTest("/home/ffengc/Project/NimbusBackup/NimbusBackup/tools/logger.hpp");
    return 0;
}