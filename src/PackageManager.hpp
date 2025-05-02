#pragma once
#include <string>
#include <vector>

class PackageManager {
public:
    PackageManager(const std::string& repoPath);
    // Install and track a package
    int installPkg(const std::string& pkgName);
    // List tracked packages
    std::vector<std::string> listPackages() const;
private:
    std::string repoPath;
};
