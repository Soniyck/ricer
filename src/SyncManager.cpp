#include "SyncManager.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <cstdlib>

namespace fs = std::filesystem;

// Utility: Replace $HOME or ~ with the user's home dir
static std::string expandHome(const std::string& path) {
    const char* home = std::getenv("HOME");
    if (!home) return path;
    if (path.rfind("$HOME", 0) == 0)
        return std::string(home) + path.substr(5);
    if (path.rfind("~", 0) == 0 && (path.size() == 1 || path[1] == '/'))
        return std::string(home) + path.substr(1);
    return path;
}

// Utility: Replace user's home dir with $HOME
static std::string abstractHome(const std::string& path) {
    const char* home = std::getenv("HOME");
    if (!home) return path;
    std::string homeStr(home);
    if (path.rfind(homeStr + "/", 0) == 0)
        return "$HOME" + path.substr(homeStr.size());
    return path;
}

SyncManager::SyncManager(const std::string& repoPath)
    : repoPath(repoPath) {}

std::string SyncManager::getSyncMapPath() const {
    return repoPath + "/.syncmap";
}

std::vector<std::tuple<std::string, std::string, std::string>> SyncManager::readSyncMap() const {
    std::vector<std::tuple<std::string, std::string, std::string>> mappings;
    std::ifstream in(getSyncMapPath());
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto sep1 = line.find('|');
        if (sep1 == std::string::npos) continue;
        auto sep2 = line.find('|', sep1 + 1);
        if (sep2 == std::string::npos) continue;
        std::string repo = line.substr(0, sep1);
        std::string local = line.substr(sep1 + 1, sep2 - sep1 - 1);
        std::string hash = line.substr(sep2 + 1);
        // Expand $HOME or ~ to actual home dir
        mappings.emplace_back(repo, expandHome(local), hash);
    }
    return mappings;
}

void SyncManager::writeSyncMap(const std::vector<std::tuple<std::string, std::string, std::string>>& mappings) const {
    std::ofstream out(getSyncMapPath());
    for (const auto& p : mappings) {
        // Abstract home dir to $HOME for portability
        out << std::get<0>(p) << "|" << abstractHome(std::get<1>(p)) << "|" << std::get<2>(p) << "\n";
    }
}

std::string SyncManager::computeSHA256(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return "";
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    char buf[4096];
    while (file.good()) {
        file.read(buf, sizeof(buf));
        SHA256_Update(&ctx, buf, file.gcount());
    }
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &ctx);
    std::ostringstream result;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        result << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return result.str();
}

int SyncManager::syncFiles(bool interactive) {
    auto mappings = readSyncMap();
    bool syncmap_changed = false;
    for (auto& [repo_file, local_file, expected_hash] : mappings) {
        std::string repo_full = repoPath + "/" + repo_file;
        std::string hash_repo = computeSHA256(repo_full);
        std::string hash_local = computeSHA256(local_file);
        if (hash_repo.empty() || hash_local.empty()) {
            std::cerr << "Missing file: " << (hash_repo.empty() ? repo_full : local_file) << std::endl;
            continue;
        }
        if (hash_repo == hash_local) {
            std::cout << repo_file << ": up-to-date" << std::endl;
            continue;
        }
        if (hash_local != expected_hash && hash_repo == expected_hash) {
            // Only local changed
            fs::copy_file(local_file, repo_full, fs::copy_options::overwrite_existing);
            std::cout << "Updated repo: " << repo_file << " <- local" << std::endl;
            expected_hash = computeSHA256(local_file);
            syncmap_changed = true;
        } else if (hash_repo != expected_hash && hash_local == expected_hash) {
            // Only repo changed
            fs::copy_file(repo_full, local_file, fs::copy_options::overwrite_existing);
            std::cout << "Updated local: " << local_file << " <- repo" << std::endl;
            expected_hash = computeSHA256(repo_full);
            syncmap_changed = true;
        } else if (hash_local != expected_hash && hash_repo != expected_hash) {
            // Both changed
            if (interactive) {
                std::cout << "Conflict: Both changed for " << repo_file << ". Overwrite local file with repo version? [y/N]: ";
                std::string resp;
                std::getline(std::cin, resp);
                if (resp == "y" || resp == "Y") {
                    fs::copy_file(repo_full, local_file, fs::copy_options::overwrite_existing);
                    std::cout << "Overwritten local: " << local_file << " <- repo (user choice)" << std::endl;
                    expected_hash = computeSHA256(repo_full);
                    syncmap_changed = true;
                } else {
                    std::cout << "Skipped: " << local_file << std::endl;
                    continue;
                }
            } else {
                std::cerr << "Conflict: Both changed for " << repo_file << ". Manual resolution needed." << std::endl;
                continue;
            }
        }
    }
    if (syncmap_changed) {
        writeSyncMap(mappings);
    }
    return 0;
}
