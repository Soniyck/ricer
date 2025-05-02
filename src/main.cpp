#include "commands.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ricer <command> [options]\n";
        return 1;
    }
    std::string cmd = argv[1];
    if (cmd == "init") {
        if (argc < 3) { std::cerr << "Usage: ricer init <repo-path>\n"; return 1; }
        return ricer::init_repo(argv[2]);
    } else if (cmd == "add-config") {
        if (argc < 3) { std::cerr << "Usage: ricer add-config <config-path>\n"; return 1; }
        return ricer::add_config(argv[2]);
    } else if (cmd == "install-pkg") {
        if (argc < 3) { std::cerr << "Usage: ricer install-pkg <package-name>\n"; return 1; }
        return ricer::install_pkg(argv[2]);
    } else if (cmd == "download") {
        return ricer::download();
    } else if (cmd == "upload") {
        return ricer::upload();
    } else if (cmd == "sync") {
        // Usage: ricer sync [--no-interactive]
        bool interactive = true;
        if (argc >= 3 && std::string(argv[2]) == "--no-interactive") interactive = false;
        return ricer::sync_files(interactive);
    } else if (cmd == "install") {
        return ricer::create_install_yaml();
    } else if (cmd == "reinstall") {
        return ricer::reinstall();
    } else {
        std::cerr << "Unknown command: " << cmd << "\n";
        return 1;
    }
}
