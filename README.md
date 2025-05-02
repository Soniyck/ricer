# ricer

A C++ CLI tool to synchronize your Linux rice (dotfiles & package setup) across devices using git as a cloud backend.

## Features
- Configure a git repo to store config and install files
- Track config files
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
ricer install                  # Interactive install command entry
ricer reinstall                # Reinstall from YAML files and auto-install packages
ricer sync [--no-interactive]  # Sync tracked files between repo and local system
ricer download                 # Download tracked configs and packages from the repo
ricer upload                   # Upload local changes to the git repo
```

### Sync Command

- `ricer sync`: Synchronizes all tracked config files between your local system and the repository. By default, prompts before overwriting files if there are changes.
- `ricer sync --no-interactive`: Runs sync without prompts, automatically updating files based on which is newer.

This command ensures your configs are up-to-date across devices. Paths in `.syncmap` are portable and will work across different usernames/machines.

## Install and Reinstall

- `ricer install`: Interactively create a YAML file of install commands in your repo's `/install` folder.
- `ricer reinstall`: Runs all new or changed YAMLs in `/install` and installs any packages listed in `/auto-install`. Keeps track of executed files and their versions in `~/.ricer-config`.

### How `ricer reinstall` Works
- Scans `/install` for YAML files. If a YAML file is new or changed (by SHA256 hash), all its commands are executed.
- Installs all packages listed in `/auto-install` (if present).
- After successful execution, updates `~/.ricer-config` with the latest hashes.

This ensures you can easily reapply or update all tracked install steps and packages after a repo update or on a new machine.

## YAML-based Install Commands

To interactively create a YAML file with install commands, use:

```sh
./ricer install
```

- Enter install commands one by one; each is executed immediately.
- After each command, confirm if it should be added to the YAML.
- Press ENTER on an empty line to finish entering commands.
- You will be prompted for the install YAML filename (extension `.yaml` is added if missing).
- The YAML file is saved to your repo's `install/` directory.

You can then use this YAML file with your install runner or for tracking purposes.
