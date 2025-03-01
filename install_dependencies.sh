#!/bin/bash

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Update package list and install dependencies
if command_exists apt-get; then
    sudo apt-get update
    sudo apt-get install -y libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev pkg-config
elif command_exists yum; then
    sudo yum check-update
    sudo yum install -y libXinerama-devel libXcursor-devel xorg-x11-server-devel mesa-libGLU-devel pkgconfig
elif command_exists pacman; then
    sudo pacman -Syu
    sudo pacman -S --needed libxinerama libxcursor xorg-server glu pkgconf
elif command_exists zypper; then
    sudo zypper refresh
    sudo zypper install -y libXinerama-devel libXcursor-devel xorg-x11-server-devel Mesa-libGLU-devel pkg-config
elif command_exists dnf; then
    sudo dnf check-update
    sudo dnf install -y libXinerama-devel libXcursor-devel xorg-x11-server-devel mesa-libGLU-devel pkgconf
elif command_exists emerge; then
    sudo emerge --sync
    sudo emerge x11-libs/libXinerama x11-libs/libXcursor x11-base/xorg-server media-libs/mesa dev-util/pkgconfig
else
    echo "Unsupported package manager. Please install the dependencies manually."
    exit 1
fi

echo "Dependencies installed successfully."
