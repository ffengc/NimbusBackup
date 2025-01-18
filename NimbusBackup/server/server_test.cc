/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// for server test

#include "service.hpp"

nimbus::DataManager* __data;
void test1() {
    // test __upload
    nimbus::Service srv;
    srv.Run();
}

int main() {
    __data = new nimbus::DataManager();
    test1();
    return 0;
}