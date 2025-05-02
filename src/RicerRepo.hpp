#pragma once
#include <string>
#include "SyncManager.hpp"
#include "ConfigManager.hpp"
#include "PackageManager.hpp"

class RicerRepo {
public:
    RicerRepo(const std::string& repoPath);

    int init();
    int download();
    int upload();

    std::string getRepoPath() const;
    SyncManager& getSyncManager();
    ConfigManager& getConfigManager();
    PackageManager& getPackageManager();
private:
    std::string repoPath;
    SyncManager syncManager;
    ConfigManager configManager;
    PackageManager packageManager;
};
