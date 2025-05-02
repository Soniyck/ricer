#!/bin/bash
set -e

# Remove old build directory if exists
rm -rf build

# Create build directory
mkdir build
cd build

# Run cmake and build
cmake ..
make