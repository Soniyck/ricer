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

# Find the ricer executable (assume it's named 'ricer')
if [ ! -f ricer ]; then
    echo "Error: ricer executable not found in build directory."
    exit 1
fi

# Install to /usr/local/bin (requires sudo)
echo "Installing ricer to /usr/local/bin (requires sudo)"
sudo cp ricer /usr/local/bin/
echo "ricer installed successfully!"
