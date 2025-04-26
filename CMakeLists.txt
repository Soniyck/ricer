cmake_minimum_required(VERSION 3.10)
project(ricer)

set(CMAKE_CXX_STANDARD 17)

add_executable(ricer
    src/main.cpp
    src/commands.cpp
    src/config.cpp
)

# Ensure output binary is always in build directory root
set_target_properties(ricer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

include_directories(include)
