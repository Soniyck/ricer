#include "config.hpp"
#include <fstream>
#include <cstdlib>

namespace ricer {
    static std::string config_file = std::string(getenv("HOME")) + "/.ricer_config";
    std::string get_repo_path() {
        std::ifstream in(config_file);
        std::string path;
        if (in >> path) return path;
        return "";
    }
    void set_repo_path(const std::string& path) {
        std::ofstream out(config_file);
        out << path;
    }
}
