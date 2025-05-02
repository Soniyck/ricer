# ricer

A C++ CLI tool to synchronize your Linux rice (dotfiles & package setup) across devices using git as a cloud backend.

## Features
- Configure a git repo to store config and install files
- Track config files
- Track installed packages
- Sync configs and packages across devices

## Dependencies
**Git** - required to use ricer. It is used to store the configuration files and install scripts in a git repository. The tool can be used with any git hosting service (e.g., GitHub, GitLab, Bitbucket).

## Build & Install

### Build from Source

```sh
# Install dependencies (Debian/Ubuntu example)
sudo apt-get install cmake g++ libyaml-cpp-dev git

# Clone the repository
git clone https://github.com/Soniyck/ricer
cd ricer

# Build
./build.sh
```

### Install

#### Option 1: Global install (requires sudo)
```sh
sudo ./install.sh
```

#### Option 2: Local install (no sudo)
```sh
./install.sh --local
```

The `install.sh` script will copy the built `ricer` binary to `/usr/local/bin` (global) or `~/bin` (local).

#### Manual install (alternative)
```sh
sudo cp build/ricer /usr/local/bin/
# or for local install
cp build/ricer ~/bin/
```

## Usage
```
ricer init <repo-path>         # Configure the git repo for storing configs and install data
ricer add-config <path>        # Track a config file
ricer install-pkg <pkg-name>   # Install a package (using apt-get) and track it
ricer install                  # Interactive install command entry
ricer reinstall                # Install all new or changed packages and commands
ricer sync [--no-interactive]  # Sync tracked config files between repo and local system
ricer download                 # Download changes from the git repo
ricer upload                   # Upload local changes to the git repo
```

## Install and Reinstall

- `ricer install`: Lets user execute commands and decide whether to track them.
- `ricer reinstall`: Runs all new or changed installations.

### Interactive Install Command

```sh
ricer install
```

- Enter install commands one by one; each is executed immediately.
- After each command, confirm if it should be tracked.
- Press ENTER on an empty line to finish entering commands.
- You will be prompted for the installation name.
- The YAML file is saved to your repo's `install/` directory.