#pragma once
#include <string>
#include <vector>

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

    // --- SYNC LOGIC ---
    /**
     * Compute the SHA-256 hash of a file.
     * @param filename Path to the file to hash
     * @return The SHA-256 hash of the file as a string
     */
    std::string compute_sha256(const std::string& filename);

    /**
     * Read the syncmap from a file.
     * @param syncmap_path Path to the syncmap file
     * @return A vector of pairs containing the filename and its corresponding hash
     */
    std::vector<std::tuple<std::string, std::string, std::string>> read_syncmap(const std::string& syncmap_path);

    /**
     * Write the syncmap to a file.
     * @param syncmap_path Path to the syncmap file
     * @param mappings A vector of pairs containing the filename and its corresponding hash
     */
    void write_syncmap(const std::string& syncmap_path, const std::vector<std::pair<std::string, std::string>>& mappings);

    /**
     * Sync files based on the syncmap.
     * @param interactive Whether to run interactively
     * @return 0 on success, nonzero on error
     */
    int sync_files(bool interactive);

    /**
     * Interactively create an install YAML file by running and collecting shell commands from the user.
     * @return 0 on success, nonzero on error
     */
    int create_install_yaml();

    // Run new or changed install YAMLs from /install
    int reinstall();
}
