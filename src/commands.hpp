#pragma once
#include <string>

namespace ricer {
    /**
     * Configure the path to the git repo folder to be used for storing configs and install data.
     * Does NOT initialize the git repo or create directories.
     * @param repo_path Path to the existing git repo folder
     * @return 0 on success, nonzero on error
     */
    int init_repo(const std::string& repo_path);

    /**
     * Add a config file to be tracked. Creates a symlink in the repo pointing to the real config file.
     * The original config file remains in place. If the repo is lost, configs are safe.
     * @param config_path Path to the config file to track
     * @return 0 on success, nonzero on error
     */
    int add_config(const std::string& config_path);

    /**
     * Install a package (via apt) and add it to the tracked install list.
     * @param pkg_name Name of the package to install
     * @return 0 on success, nonzero on error
     */
    int install_pkg(const std::string& pkg_name);

    /**
     * Download tracked configs and packages from the repo to the local machine.
     * @return 0 on success, nonzero on error
     */
    int download();

    /**
     * Upload local changes to the git repo (commit and push).
     * @return 0 on success, nonzero on error
     */
    int upload();

    /**
     * Helper: Check if the repo is configured (ricer init ran).
     * @param repo_path (output) Set to the repo path if configured.
     * @return true if configured, false otherwise
     */
    bool check_repo_configured(std::string& repo_path);
}
