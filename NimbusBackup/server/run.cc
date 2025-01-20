/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// for server

#include "service.hpp"

nimbus::DataManager* __data;
void run() {
    nimbus::Service srv;
    srv.Run();
}

int main() {
    __data = new nimbus::DataManager();
    run();
    return 0;
}