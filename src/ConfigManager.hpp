#pragma once
#include <string>
#include <vector>

class ConfigManager {
public:
    ConfigManager(const std::string& repoPath);
    // Add a config file
    int addConfig(const std::string& configPath);
    // List tracked configs
    std::vector<std::string> listConfigs() const;
private:
    std::string repoPath;
};
