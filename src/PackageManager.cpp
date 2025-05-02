#include "PackageManager.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

PackageManager::PackageManager(const std::string& repoPath)
    : repoPath(repoPath) {}

int PackageManager::installPkg(const std::string& pkgName) {
    if (repoPath.empty()) {
        std::cerr << "Repo not configured. Run 'ricer init <repo-path>' first." << std::endl;
        return 1;
    }
    std::string cmd = "sudo apt-get install " + pkgName + " -y";
    int ret = system(cmd.c_str());
    if (ret != 0) {
        std::cerr << "Failed to install package: " << pkgName << std::endl;
        return 1;
    }
    std::string auto_install = repoPath + "/auto-install";
    bool already_listed = false;
    {
        std::ifstream in(auto_install);
        std::string line;
        while (std::getline(in, line)) {
            if (line == pkgName) {
                already_listed = true;
                break;
            }
        }
    }
    if (!already_listed) {
        std::ofstream out(auto_install, std::ios::app);
        out << pkgName << std::endl;
        std::cout << "Installed and tracked package: " << pkgName << std::endl;
    } else {
        std::cout << "Package already tracked: " << pkgName << std::endl;
    }
    return 0;
}

std::vector<std::string> PackageManager::listPackages() const {
    std::vector<std::string> pkgs;
    std::string auto_install = repoPath + "/auto-install";
    std::ifstream in(auto_install);
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty()) pkgs.push_back(line);
    }
    return pkgs;
}
