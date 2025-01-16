/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// for bundle lib test

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