#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <yaml-cpp/yaml.h>

std::vector<std::string> load_install_commands(const std::string& filename) {
    std::vector<std::string> commands;
    YAML::Node config = YAML::LoadFile(filename);
    if (config["install"]) {
        for (const auto& cmd : config["install"]) {
            commands.push_back(cmd.as<std::string>());
        }
    }
    return commands;
}