#!/bin/bash

echo "Installing Switch Music Player dependencies..."

# Update package list
sudo dkp-pacman -Sy

# Install base requirements
echo "Installing switch-dev..."
sudo dkp-pacman -S --needed --noconfirm switch-dev

# Install networking and audio libs
echo "Installing libraries..."
sudo dkp-pacman -S --needed --noconfirm \
    switch-curl \
    switch-mbedtls \
    switch-zlib \
    switch-examples

# Install libtesla for overlay (if not already installed)
echo "Installing libTesla..."
if [ ! -d "libs/libtesla" ]; then
    mkdir -p libs
    cd libs
    git clone https://github.com/WerWolv/libtesla.git
    cd libtesla
    make
    cd ../..
fi

echo "Dependencies installed!"
echo ""
echo "Next steps:"
echo "1. Run 'make' to build the project"
echo "2. Copy files to your SD card"
echo "3. Restart your Switch"