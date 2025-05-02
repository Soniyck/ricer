#include "commands.hpp"
#include "config.hpp"
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include "RicerRepo.hpp"
#include "ConfigManager.hpp"
#include "PackageManager.hpp"
#include "InstallManager.hpp"

namespace fs = std::filesystem;

namespace ricer {
    static std::unique_ptr<RicerRepo> global_repo;

    int init_repo(const std::string& repo_path) {
        global_repo = std::make_unique<RicerRepo>(repo_path);
        return global_repo->init();
    }

    int add_config(const std::string& config_path) {
        if (!global_repo) global_repo = std::make_unique<RicerRepo>(get_repo_path());
        return global_repo->getConfigManager().addConfig(config_path);
    }

    int install_pkg(const std::string& pkg_name) {
        if (!global_repo) global_repo = std::make_unique<RicerRepo>(get_repo_path());
        return global_repo->getPackageManager().installPkg(pkg_name);
    }

    int download() {
        if (!global_repo) global_repo = std::make_unique<RicerRepo>(get_repo_path());
        return global_repo->download();
    }

    int upload() {
        if (!global_repo) global_repo = std::make_unique<RicerRepo>(get_repo_path());
        return global_repo->upload();
    }

    int sync_files(bool interactive) {
        if (!global_repo) global_repo = std::make_unique<RicerRepo>(get_repo_path());
        return global_repo->getSyncManager().syncFiles(interactive);
    }

    int create_install_yaml() {
        if (!global_repo) global_repo = std::make_unique<RicerRepo>(get_repo_path());
        InstallManager mgr(global_repo->getRepoPath());
        return mgr.createInstallYaml();
    }

    int reinstall() {
        if (!global_repo) global_repo = std::make_unique<RicerRepo>(get_repo_path());
        InstallManager mgr(global_repo->getRepoPath());
        return mgr.reinstall();
    }
}
