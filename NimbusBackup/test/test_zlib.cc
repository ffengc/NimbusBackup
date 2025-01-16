/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// for zlib test

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