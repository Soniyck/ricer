#pragma once
#include <string>
#include <vector>
#include <tuple>

class SyncManager {
public:
    SyncManager(const std::string& repoPath);

    // Loads mappings from .syncmap
    std::vector<std::tuple<std::string, std::string, std::string>> readSyncMap() const;
    // Writes mappings to .syncmap
    void writeSyncMap(const std::vector<std::tuple<std::string, std::string, std::string>>& mappings) const;
    // Syncs files according to .syncmap
    int syncFiles(bool interactive = true);
    // Computes SHA256 hash of a file
    static std::string computeSHA256(const std::string& filename);

    std::string getSyncMapPath() const;
private:
    std::string repoPath;
};
