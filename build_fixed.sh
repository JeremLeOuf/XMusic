#!/bin/bash

echo "======================================"
echo "   XMusic Fixed Build (Working)       "
echo "======================================"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Setup paths
export PATH=$DEVKITPRO/devkitA64/bin:$PATH

# Clean and prepare
echo -e "${YELLOW}Preparing build...${NC}"
cd sysmodule
rm -rf build *.elf *.nso
mkdir -p build

# Copy fixed files
echo "  Copying fixed source files..."
cp source/audio_manager_fixed.h source/audio_manager.h
cp source/main_minimal.cpp source/main.cpp

# Compile with exceptions disabled (embedded requirement)
echo -e "${YELLOW}Compiling XMusic...${NC}"

echo "  Compiling main.cpp..."
aarch64-none-elf-g++ \
    -g -Wall -O2 -ffunction-sections \
    -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE \
    -I../common \
    -I$DEVKITPRO/libnx/include \
    -I$DEVKITPRO/portlibs/switch/include \
    -D__SWITCH__ \
    -fno-rtti -fno-exceptions \
    -std=gnu++17 \
    -c source/main.cpp -o build/main.o

if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation failed!${NC}"
    cd ..
    exit 1
fi

# Link
echo "  Linking..."
aarch64-none-elf-g++ \
    -specs=$DEVKITPRO/libnx/switch.specs \
    -g \
    -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE \
    -Wl,-Map,xmusic.map \
    build/main.o \
    -L$DEVKITPRO/libnx/lib \
    -L$DEVKITPRO/portlibs/switch/lib \
    -lnx \
    -lm \
    -lpthread \
    -o xmusic.elf

if [ $? -ne 0 ]; then
    echo -e "${RED}Linking failed!${NC}"
    cd ..
    exit 1
fi

# Create NSO
echo "  Creating NSO..."
elf2nso xmusic.elf xmusic.nso

if [ $? -ne 0 ]; then
    echo -e "${RED}NSO creation failed!${NC}"
    cd ..
    exit 1
fi

cd ..

# Create distribution
echo -e "${YELLOW}Creating distribution...${NC}"

rm -rf dist
mkdir -p dist/atmosphere/contents/58000000000000A1/flags
mkdir -p dist/config/xmusic

# Copy NSO
cp sysmodule/xmusic.nso dist/atmosphere/contents/58000000000000A1/exefs.nso
echo -e "${GREEN}  ✅ Sysmodule built and copied${NC}"

# Create boot flag
touch dist/atmosphere/contents/58000000000000A1/flags/boot2.flag

# Create toolbox.json
cat > dist/atmosphere/contents/58000000000000A1/toolbox.json << 'JSON_END'
{
    "name": "XMusic",
    "tid": "58000000000000A1",
    "requires_reboot": false,
    "version": "0.1.1"
}
JSON_END

# Create config
cat > dist/config/xmusic/config.json << 'JSON_END'
{
    "default_volume": 0.3,
    "startup_sound": true,
    "test_tone_frequency": 440
}
JSON_END

# Summary
echo ""
echo -e "${GREEN}======================================"
echo "     ✅ XMusic Build Success!         "
echo "======================================${NC}"
echo ""
echo "Build summary:"
ls -lh sysmodule/xmusic.nso
echo ""
echo "Distribution files:"
ls -la dist/atmosphere/contents/58000000000000A1/
echo ""
echo "Features:"
echo "  • Startup melody (Mario coin style)"
echo "  • Test tone (440Hz)"
echo "  • Volume control"
echo "  • Play/Pause"
echo "  • IPC service"
echo ""
echo "To install:"
echo "  1. Copy dist/* to SD card root"
echo "  2. Restart Switch"
echo "  3. You'll hear the startup melody!"
echo ""
echo -e "${YELLOW}Note: This is the audio-only version.${NC}"
echo -e "${YELLOW}Streaming will be added after testing.${NC}"