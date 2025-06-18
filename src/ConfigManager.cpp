#include "ConfigManager.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "SyncManager.hpp"

namespace fs = std::filesystem;

ConfigManager::ConfigManager(const std::string& repoPath)
    : repoPath(repoPath) {}

// Single file tracking helper
int ConfigManager::addConfigFile(const std::string& configPath) {
    fs::path src(configPath);
    fs::path dest = fs::path(repoPath) / src.filename();
    // Copy the file content into the repo
    try {
        fs::copy_file(src, dest, fs::copy_options::overwrite_existing);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Failed to copy file to repo: " << e.what() << std::endl;
        return 1;
    }
    // Update .syncmap with mapping and hash
    SyncManager syncManager(repoPath);
    auto mappings = syncManager.readSyncMap();
    std::string hash = SyncManager::computeSHA256(configPath);
    // Remove any existing mapping for this repo file or local file
    mappings.erase(std::remove_if(mappings.begin(), mappings.end(), [&](const auto& tup) {
        return std::get<0>(tup) == dest.filename() || std::get<1>(tup) == src;
    }), mappings.end());
    mappings.emplace_back(dest.filename(), src.string(), hash);
    syncManager.writeSyncMap(mappings);
    std::cout << "Tracked config (copied to repo and mapped in .syncmap): " << dest << " <-> " << src << std::endl;
    return 0;
}

// Folder tracking helper
int ConfigManager::addConfigFolder(const std::string& folderPath) {
    int errors = 0;
    for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
        if (fs::is_regular_file(entry)) {
            errors += addConfigFile(entry.path().string());
        }
    }
    return errors;
}

// Public method dispatches to file/folder helper
int ConfigManager::addConfig(const std::string& configPath) {
    if (repoPath.empty()) {
        std::cerr << "Repo not configured. Run 'ricer init <repo-path>' first." << std::endl;
        return 1;
    }
    fs::path src(configPath);
    if (!fs::exists(src)) {
        std::cerr << "Config file or folder does not exist: " << configPath << std::endl;
        return 1;
    }
    if (fs::is_directory(src)) {
        return addConfigFolder(configPath);
    } else {
        return addConfigFile(configPath);
    }
}

std::vector<std::string> ConfigManager::listConfigs() const {
    std::vector<std::string> configs;
    for (const auto& entry : fs::directory_iterator(repoPath)) {
        if (fs::is_symlink(entry)) {
            configs.push_back(entry.path().filename());
        }
    }
    return configs;
}
