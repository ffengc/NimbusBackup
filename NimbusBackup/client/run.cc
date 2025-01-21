/*
 * Write by Yufc
 * See https://github.com/ffengc/NimbusBackup
 * please cite my project link: https://github.com/ffengc/NimbusBackup when you use this code
 */

// run.cc

#include "client.hpp"
#include "data_manager.hpp"
#include <string>
#include <vector>

#define BACKUP_FILE "./backup.dat"
#define BACKUP_DIR "./backup/"
int main() {
    nimbus::BackUp backup(BACKUP_DIR, BACKUP_FILE);
    backup.run();
    return 0;
}