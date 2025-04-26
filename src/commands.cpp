#include "commands.hpp"
#include "config.hpp"
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <algorithm>

namespace fs = std::filesystem;

namespace ricer {
    /**
     * Initializes the repository by setting the path in the config.
     * 
     * @param repo_path The path to the repository.
     * @return 0 on success, 1 on failure.
     */
    int init_repo(const std::string& repo_path) {
        // Only set the repo path in config, do not create directories or git init
        if (!fs::exists(repo_path) || !fs::is_directory(repo_path)) {
            std::cerr << "Given repo path does not exist or is not a directory." << std::endl;
            return 1;
        }
        set_repo_path(repo_path);
        std::cout << "Configured repo path: " << repo_path << std::endl;
        return 0;
    }

    /**
     * Adds a config file to the repository by creating a symlink.
     * 
     * @param config_path The path to the config file.
     * @return 0 on success, 1 on failure.
     */
    int add_config(const std::string& config_path) {
        std::string repo_path = get_repo_path();
        if (repo_path.empty()) {
            std::cerr << "Repo not configured. Run 'ricer init <repo-path>' first." << std::endl;
            return 1;
        }
        fs::path src(config_path);
        if (!fs::exists(src)) {
            std::cerr << "Config file does not exist: " << config_path << std::endl;
            return 1;
        }
        // Create a symlink in the repo pointing to the real config file
        fs::path dest = fs::path(repo_path) / src.filename();
        // Remove if a file/symlink already exists at dest
        if (fs::exists(dest)) {
            fs::remove(dest);
        }
        if (symlink(src.c_str(), dest.c_str()) != 0) {
            std::cerr << "Failed to create symlink in repo. (Maybe not enough permissions?)" << std::endl;
            return 1;
        }
        std::cout << "Tracked config (symlink in repo -> actual file): " << dest << " -> " << src << std::endl;
        return 0;
    }

    /**
     * Installs a package and tracks it in the repository.
     * 
     * @param pkg_name The name of the package to install.
     * @return 0 on success, 1 on failure.
     */
    int install_pkg(const std::string& pkg_name) {
        std::string repo_path = get_repo_path();
        if (repo_path.empty()) {
            std::cerr << "Repo not configured. Run 'ricer init <repo-path>' first." << std::endl;
            return 1;
        }
        // Install package
        std::string cmd = "sudo apt-get install " + pkg_name + " -y";
        int ret = system(cmd.c_str());
        if (ret != 0) {
            std::cerr << "Failed to install package: " << pkg_name << std::endl;
            return 1;
        }
        // Check if already in auto-install
        std::string auto_install = repo_path + "/auto-install";
        bool already_listed = false;
        {
            std::ifstream in(auto_install);
            std::string line;
            while (std::getline(in, line)) {
                if (line == pkg_name) {
                    already_listed = true;
                    break;
                }
            }
        }
        if (!already_listed) {
            std::ofstream out(auto_install, std::ios::app);
            out << pkg_name << std::endl;
            std::cout << "Installed and tracked package: " << pkg_name << std::endl;
        } else {
            std::cout << "Package already tracked: " << pkg_name << std::endl;
        }
        return 0;
    }

    int download() {
        std::string repo_path = get_repo_path();
        if (repo_path.empty()) {
            std::cerr << "Repo not configured. Run 'ricer init <repo-path>' first." << std::endl;
            return 1;
        }
        // 1. Pull latest from git
        std::cout << "Pulling latest changes from git..." << std::endl;
        std::string git_pull = "git -C '" + repo_path + "' pull";
        int git_result = system(git_pull.c_str());
        if (git_result != 0) {
            std::cerr << "Warning: git pull failed or there were no updates." << std::endl;
        }

        // 2. Check symlinks in repo
        std::cout << "Checking tracked config symlinks..." << std::endl;
        for (const auto& entry : fs::directory_iterator(repo_path)) {
            if (fs::is_symlink(entry)) {
                fs::path link = entry.path();
                fs::path target = fs::read_symlink(link);
                if (!fs::exists(target)) {
                    // Try to copy file from repo to target location
                    std::cout << "Target missing for " << link.filename() << " -> " << target << ". Attempting to restore..." << std::endl;
                    try {
                        fs::copy_file(link, target, fs::copy_options::overwrite_existing);
                        std::cout << "Restored: " << target << std::endl;
                    } catch (const fs::filesystem_error& e) {
                        std::cerr << "Failed to restore config for " << link.filename() << ": " << e.what() << std::endl;
                    }
                }
            }
        }

        // 3. Install packages (always run install, apt will skip already-installed)
        std::string auto_install = repo_path + "/auto-install";
        std::ifstream in(auto_install);
        if (!in) {
            std::cerr << "No auto-install file found in repo." << std::endl;
        } else {
            std::cout << "Installing packages from auto-install..." << std::endl;
            std::string pkg;
            while (std::getline(in, pkg)) {
                if (pkg.empty()) continue;
                std::cout << "Installing: " << pkg << std::endl;
                std::string install_cmd = "sudo apt-get install " + pkg + " -y";
                int ret = system(install_cmd.c_str());
                if (ret != 0) {
                    std::cerr << "Failed to install package: " << pkg << std::endl;
                }
            }
        }
        std::cout << "Download complete." << std::endl;
        return 0;
    }

    int upload() {
        std::string repo_path = get_repo_path();
        if (repo_path.empty()) {
            std::cerr << "Repo not configured. Run 'ricer init <repo-path>' first." << std::endl;
            return 1;
        }
        // Stage all changes
        std::string add_cmd = "git -C '" + repo_path + "' add .";
        int add_result = system(add_cmd.c_str());
        if (add_result != 0) {
            std::cerr << "git add failed." << std::endl;
            return 1;
        }
        // Commit
        std::string commit_cmd = "git -C '" + repo_path + "' commit -m 'ricer: auto upload'";
        int commit_result = system(commit_cmd.c_str());
        if (commit_result != 0) {
            std::cerr << "git commit failed (maybe nothing to commit)." << std::endl;
            // Not a fatal error, continue
        }
        // Push
        std::string push_cmd = "git -C '" + repo_path + "' push";
        int push_result = system(push_cmd.c_str());
        if (push_result != 0) {
            std::cerr << "git push failed." << std::endl;
            return 1;
        }
        std::cout << "Upload complete." << std::endl;
        return 0;
    }
}
