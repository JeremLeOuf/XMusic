#!/bin/bash

# XMusic Controller Build Script

export DEVKITPRO=/opt/devkitpro
export DEVKITA64=$DEVKITPRO/devkitA64
export PATH=$DEVKITPRO/tools/bin:$DEVKITA64/bin:$PATH

echo "======================================="
echo "    XMusic Controller Build           "
echo "======================================="

# Clean previous build
echo "Cleaning previous build..."
rm -f XMusicController.elf XMusicController.nro XMusicController.nacp

# Create build directory
mkdir -p build

# Compile controller
echo "Compiling XMusic Controller..."
aarch64-none-elf-g++ \
    -MMD -MP -MF build/main.d \
    -g -Wall -O2 -ffunction-sections \
    -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE \
    -I$DEVKITPRO/libnx/include \
    -I$DEVKITPRO/portlibs/switch/include \
    -I../common \
    -D__SWITCH__ \
    -std=c++17 \
    -c source/main.cpp -o build/main.o

if [ $? -ne 0 ]; then
    echo "❌ Compilation failed!"
    exit 1
fi

# Link
echo "Linking..."
aarch64-none-elf-g++ \
    -specs=$DEVKITPRO/libnx/switch.specs \
    -g \
    -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE \
    build/main.o \
    -L$DEVKITPRO/libnx/lib \
    -L$DEVKITPRO/portlibs/switch/lib \
    -lnx \
    -lm \
    -o XMusicController.elf

if [ $? -ne 0 ]; then
    echo "❌ Linking failed!"
    exit 1
fi

# Create NACP
echo "Creating NACP..."
nacptool --create "XMusic Controller" "JeremLeOuf" "0.1.0" XMusicController.nacp

# Convert to NRO
echo "Creating NRO..."
elf2nro XMusicController.elf XMusicController.nro --nacp=XMusicController.nacp

if [ $? -ne 0 ]; then
    echo "❌ NRO creation failed!"
    exit 1
fi

echo ""
echo "======================================="
echo "   ✅ Controller Build Success!       "
echo "======================================="
echo ""
echo "Output files:"
ls -la XMusicController.nro XMusicController.elf 2>/dev/null
echo ""
echo "To test:"
echo "  1. Install XMusic sysmodule first"
echo "  2. Copy XMusicController.nro to Switch SD:/switch/"
echo "  3. Run via Homebrew Launcher"
echo "  4. Use gamepad controls to interact with XMusic"
echo ""
echo "Controls:"
echo "  A - Play/Pause"
echo "  X - Next Track"  
echo "  Y - Previous Track"
echo "  L/R - Volume Down/Up"
echo "  ZL - Get Status"
echo "  + - Exit"
