# ricer

A C++ CLI tool to synchronize your Linux rice (dotfiles & package setup) across devices using git as a cloud backend.

## Features (MVP)
- Configure a git repo to store config and install files
- Track config files (symlink from config location to repo)
- Track installed packages
- Sync configs and packages across devices

## Build & Install

To clean build and install `ricer` globally (requires sudo):
```sh
./build_and_install.sh
```
This script will:
- Remove any previous build directory
- Build the project using CMake
- Copy the `ricer` executable to `/usr/local/bin` (prompting for your sudo password)

If you prefer to install manually:
```sh
mkdir build && cd build
cmake ..
make
sudo cp ricer /usr/local/bin/
```

If you want a local install (no sudo), copy `build/ricer` to a directory in your `$PATH` (e.g., `~/bin`).

## Usage
```
ricer init <repo-path>         # Configure the git repo for storing configs and install data
ricer add-config <path>        # Track a config file (symlink in repo)
ricer install-pkg <pkg-name>   # Install a package and track it
ricer download                 # Download tracked configs and packages from the repo
ricer upload                   # Upload local changes to the git repo
```
