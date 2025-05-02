#pragma once
#include <string>
#include <map>
#include <vector>

class RicerConfig {
public:
    std::map<std::string, std::string> getInstallHashes() const;
    void setInstallHash(const std::string& filename, const std::string& hash);
    std::vector<std::string> getAutoInstallPackages(const std::string& repoPath) const;
};

class InstallManager {
public:
    InstallManager(const std::string& repoPath);
    // Interactive creation of install YAML
    int createInstallYaml();
    // Run new/changed install YAMLs and auto-install
    int reinstall();
private:
    std::string repoPath;
    RicerConfig config;
    void updateInstallLog(const std::string& filename, const std::string& fullpath);
};
