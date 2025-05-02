#include "InstallManager.hpp"
#include "SyncManager.hpp"
#include "InstallRunner.hpp"
#include "config.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cstdlib>

namespace fs = std::filesystem;

std::map<std::string, std::string> RicerConfig::getInstallHashes() const {
    std::map<std::string, std::string> hashes;
    std::ifstream in(ricer::config_file);
    std::string line;
    while (std::getline(in, line)) {
        if (line.rfind("install:", 0) == 0) {
            size_t sep = line.find(' ');
            if (sep != std::string::npos) {
                std::string fname = line.substr(8, sep-8);
                std::string hash = line.substr(sep+1);
                hashes[fname] = hash;
            }
        }
    }
    return hashes;
}

void RicerConfig::setInstallHash(const std::string& filename, const std::string& hash) {
    std::ifstream in(ricer::config_file);
    std::vector<std::string> lines;
    bool found = false;
    std::string tag = "install:" + filename;
    std::string line;
    while (std::getline(in, line)) {
        if (line.rfind("install:", 0) == 0 && line.find(filename) != std::string::npos) {
            lines.push_back("install:" + filename + " " + hash);
            found = true;
        } else {
            lines.push_back(line);
        }
    }
    if (!found) {
        lines.push_back("install:" + filename + " " + hash);
    }
    std::ofstream out(ricer::config_file);
    for (const auto& l : lines) out << l << "\n";
}

std::vector<std::string> RicerConfig::getAutoInstallPackages(const std::string& repoPath) const {
    std::vector<std::string> pkgs;
    std::ifstream in(repoPath + "/auto-install");
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty()) pkgs.push_back(line);
    }
    return pkgs;
}

// --- InstallManager ---
InstallManager::InstallManager(const std::string& repoPath)
    : repoPath(repoPath) {}

int InstallManager::createInstallYaml() {
    std::vector<std::string> commands;
    std::string input;
    std::cout << "Enter install commands one by one. Each will be executed immediately.\n"
                 "After execution, you'll be asked if it should be added to the YAML.\n"
                 "Press ENTER on an empty line to finish.\n";
    while (true) {
        std::cout << "\nCommand> ";
        std::getline(std::cin, input);
        if (input.empty()) break;
        int ret = std::system(input.c_str());
        if (ret != 0) {
            std::cout << "Command failed (exit code " << ret << "). Add anyway? [y/N]: ";
        } else {
            std::cout << "Command succeeded. Add to install file? [Y/n]: ";
        }
        std::string resp;
        std::getline(std::cin, resp);
        if ((ret == 0 && (resp.empty() || resp == "y" || resp == "Y")) ||
            (ret != 0 && (resp == "y" || resp == "Y"))) {
            commands.push_back(input);
        }
    }
    if (commands.empty()) {
        std::cout << "No commands to write. Aborting.\n";
        return 1;
    }
    std::string filename;
    std::cout << "\nEnter name for the install YAML file (default: install.yaml): ";
    std::getline(std::cin, filename);
    if (filename.empty()) filename = "install.yaml";
    std::string dir = repoPath + "/install/";
    std::string mkdir_cmd = "mkdir -p '" + dir + "'";
    std::system(mkdir_cmd.c_str());
    std::ofstream out(dir + filename);
    if (!out) {
        std::cerr << "Failed to open file for writing: " << dir + filename << "\n";
        return 1;
    }
    out << "install:\n";
    for (const auto& cmd : commands) {
        out << "  - " << cmd << "\n";
    }
    out.close();

    // Compute hash and set it in config
    std::string fullpath = dir + filename;
    updateInstallLog(filename, fullpath);

    std::cout << "Install YAML created: install/" << filename << "\n";
    return 0;
}

int InstallManager::reinstall() {
    bool anyRun = false;
    // 1. YAMLs
    std::map<std::string, std::string> prevHashes = config.getInstallHashes();
    std::string installDir = repoPath + "/install";
    for (const auto& entry : fs::directory_iterator(installDir)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".yaml") continue;
        std::string fname = entry.path().filename();
        std::string fullpath = entry.path();
        std::string hash = SyncManager::computeSHA256(fullpath);
        if (prevHashes[fname] != hash) {
            std::cout << "[ricer] Running install: " << fname << std::endl;
            auto cmds = load_install_commands(fullpath);
            for (const auto& cmd : cmds) {
                std::cout << "Running: " << cmd << std::endl;
                int ret = std::system(cmd.c_str());
                if (ret != 0) {
                    std::cerr << "Command failed: " << cmd << std::endl;
                    return ret;
                }
            }
            updateInstallLog(fname, fullpath);
            anyRun = true;
        }
    }
    // 2. auto-install
    auto pkgs = config.getAutoInstallPackages(repoPath);
    for (const auto& pkg : pkgs) {
        std::string cmd = "sudo apt-get install " + pkg + " -y";
        std::cout << "Auto-install: " << cmd << std::endl;
        int ret = std::system(cmd.c_str());
        if (ret != 0) {
            std::cerr << "Failed to install package: " << pkg << std::endl;
            return ret;
        }
        anyRun = true;
    }
    if (!anyRun) {
        std::cout << "No new or changed install YAMLs or packages to run." << std::endl;
    }
    return 0;
}

void InstallManager::updateInstallLog(const std::string& filename, const std::string& fullpath) {
    std::string hash = SyncManager::computeSHA256(fullpath);
    config.setInstallHash(filename, hash);
    std::cout << "Install hash for 'install/" << filename << "': " << hash << std::endl;
}
