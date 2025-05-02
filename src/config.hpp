#pragma once
#include <string>

namespace ricer {
    static std::string config_file = std::string(getenv("HOME")) + "/.ricer_config";
    std::string get_repo_path();
    void set_repo_path(const std::string& path);
}
