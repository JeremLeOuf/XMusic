#!/bin/bash

echo "======================================"
echo "     XMusic Simple Build (WSL)        "
echo "======================================"

# Set devkitPro paths explicitly
export PATH=$DEVKITPRO/devkitA64/bin:$PATH
export DEVKITA64=$DEVKITPRO/devkitA64

# Clean
echo "Cleaning..."
rm -rf sysmodule/build
rm -f sysmodule/*.elf sysmodule/*.nso

# Create build directory
mkdir -p sysmodule/build

# Compile
echo "Compiling main.cpp..."
cd sysmodule
aarch64-none-elf-g++ \
    -g -Wall -O2 -ffunction-sections \
    -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE \
    -I../common \
    -I$DEVKITPRO/libnx/include \
    -I$DEVKITPRO/portlibs/switch/include \
    -D__SWITCH__ \
    -fno-rtti -fno-exceptions -std=gnu++17 \
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
    -Wl,-Map,xmusic.map \
    build/main.o \
    -L$DEVKITPRO/libnx/lib \
    -L$DEVKITPRO/portlibs/switch/lib \
    -lnx \
    -o xmusic.elf

if [ $? -ne 0 ]; then
    echo "❌ Linking failed!"
    exit 1
fi

# Convert to NSO
echo "Creating NSO..."
elf2nso xmusic.elf xmusic.nso

if [ $? -ne 0 ]; then
    echo "❌ NSO creation failed!"
    exit 1
fi

cd ..

# Create distribution
echo "Creating distribution..."
rm -rf dist
mkdir -p dist/atmosphere/contents/58000000000000A1/flags
mkdir -p dist/config/xmusic

cp sysmodule/xmusic.nso dist/atmosphere/contents/58000000000000A1/exefs.nso
touch dist/atmosphere/contents/58000000000000A1/flags/boot2.flag

# Create toolbox.json
cat > dist/atmosphere/contents/58000000000000A1/toolbox.json << 'JSON_END'
{
    "name": "XMusic",
    "tid": "58000000000000A1",
    "requires_reboot": false,
    "version": "0.1.0-alpha"
}
JSON_END

echo ""
echo "======================================"
echo "✅ Build successful!"
echo "======================================"
echo ""
echo "Files created:"
ls -la dist/atmosphere/contents/58000000000000A1/
echo ""
echo "To install:"
echo "1. Copy dist/ contents to SD card root"
echo "2. Restart Switch"
