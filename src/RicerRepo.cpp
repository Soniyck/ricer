#include "RicerRepo.hpp"
#include "config.hpp"
#include "InstallManager.hpp"
#include "ConfigManager.hpp"
#include "PackageManager.hpp"
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <unistd.h>

namespace fs = std::filesystem;

RicerRepo::RicerRepo(const std::string& repoPath)
    : repoPath(repoPath), syncManager(repoPath), configManager(repoPath), packageManager(repoPath) {}

std::string RicerRepo::getRepoPath() const {
    return repoPath;
}

SyncManager& RicerRepo::getSyncManager() {
    return syncManager;
}

ConfigManager& RicerRepo::getConfigManager() {
    return configManager;
}

PackageManager& RicerRepo::getPackageManager() {
    return packageManager;
}

int RicerRepo::init() {
    if (!fs::exists(repoPath) || !fs::is_directory(repoPath)) {
        std::cerr << "Given repo path does not exist or is not a directory." << std::endl;
        return 1;
    }
    ricer::set_repo_path(repoPath);
    std::cout << "Configured repo path: " << repoPath << std::endl;
    return 0;
}

int RicerRepo::download() {
    // 1. Pull latest from git
    std::cout << "Pulling latest changes from git..." << std::endl;
    std::string git_pull = "git -C '" + repoPath + "' pull";
    int git_result = system(git_pull.c_str());
    if (git_result != 0) {
        std::cerr << "Warning: git pull failed or there were no updates." << std::endl;
    }
    // 2. Sync files
    std::cout << "Syncing files using .syncmap..." << std::endl;
    syncManager.syncFiles();
    // 3. Run install YAMLs and auto-install
    std::cout << "Running install YAMLs and auto-install..." << std::endl;
    InstallManager installMgr(repoPath);
    installMgr.reinstall();
    std::cout << "Download complete." << std::endl;
    return 0;
}

int RicerRepo::upload() {
    std::string add_cmd = "git -C '" + repoPath + "' add .";
    int add_result = system(add_cmd.c_str());
    if (add_result != 0) {
        std::cerr << "git add failed." << std::endl;
        return 1;
    }
    std::string commit_cmd = "git -C '" + repoPath + "' commit -m 'ricer: auto upload'";
    int commit_result = system(commit_cmd.c_str());
    if (commit_result != 0) {
        std::cerr << "git commit failed (maybe nothing to commit)." << std::endl;
    }
    std::string push_cmd = "git -C '" + repoPath + "' push";
    int push_result = system(push_cmd.c_str());
    if (push_result != 0) {
        std::cerr << "git push failed." << std::endl;
        return 1;
    }
    std::cout << "Upload complete." << std::endl;
    return 0;
}
