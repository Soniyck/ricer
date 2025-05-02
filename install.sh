#!/bin/bash
set -e

INSTALL_DIR="/usr/local/bin"
USE_LOCAL=false

# Parse arguments
for arg in "$@"; do
    case $arg in
        --local)
            USE_LOCAL=true
            INSTALL_DIR="$HOME/bin"
            ;;
    esac
done

cd build

# Find the ricer executable (assume it's named 'ricer')
if [ ! -f ricer ]; then
    echo "Error: ricer executable not found in build directory."
    exit 1
fi

if [ "$USE_LOCAL" = true ]; then
    # Create ~/bin if it doesn't exist
    mkdir -p "$INSTALL_DIR"
    echo "Installing ricer to $INSTALL_DIR (no sudo required)"
    cp ricer "$INSTALL_DIR/"
else
    echo "Installing ricer to $INSTALL_DIR (requires sudo)"
    sudo cp ricer "$INSTALL_DIR/"
fi

echo "ricer installed successfully!"
