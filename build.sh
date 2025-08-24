#!/bin/bash

echo "======================================"
echo "   XMusic Complete Build System       "
echo "======================================"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check devkitPro
if [ -z "$DEVKITPRO" ]; then
    echo -e "${RED}Error: devkitPro not installed${NC}"
    exit 1
fi

# Setup paths for WSL
export PATH=$DEVKITPRO/devkitA64/bin:$PATH
export DEVKITA64=$DEVKITPRO/devkitA64

# Function to build sysmodule
build_sysmodule() {
    echo -e "${YELLOW}Building sysmodule...${NC}"
    
    cd sysmodule
    rm -rf build *.elf *.nso
    mkdir -p build
    
    # Compile audio_manager
    echo "  Compiling audio_manager.h..."
    aarch64-none-elf-g++ \
        -g -Wall -O2 -ffunction-sections \
        -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE \
        -I../common -I$DEVKITPRO/libnx/include \
        -D__SWITCH__ -std=gnu++17 -fno-rtti -fno-exceptions \
        -x c++ -c source/audio_manager.h -o build/audio_manager.o 2>/dev/null
    
    # Compile ipc_service
    echo "  Compiling ipc_service.h..."
    aarch64-none-elf-g++ \
        -g -Wall -O2 -ffunction-sections \
        -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE \
        -I../common -I$DEVKITPRO/libnx/include \
        -D__SWITCH__ -std=gnu++17 -fno-rtti -fno-exceptions \
        -x c++ -c source/ipc_service.h -o build/ipc_service.o 2>/dev/null
    
    # Compile main
    echo "  Compiling main.cpp..."
    aarch64-none-elf-g++ \
        -g -Wall -O2 -ffunction-sections \
        -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE \
        -I../common -I$DEVKITPRO/libnx/include \
        -D__SWITCH__ -std=gnu++17 -fno-rtti -fno-exceptions \
        -c source/main.cpp -o build/main.o
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}  Compilation failed!${NC}"
        cd ..
        return 1
    fi
    
    # Link
    echo "  Linking..."
    aarch64-none-elf-g++ \
        -specs=$DEVKITPRO/libnx/switch.specs \
        -g -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE \
        -Wl,-Map,xmusic.map \
        build/main.o \
        -L$DEVKITPRO/libnx/lib \
        -lnx -lm -lpthread \
        -o xmusic.elf
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}  Linking failed!${NC}"
        cd ..
        return 1
    fi
    
    # Create NSO
    echo "  Creating NSO..."
    elf2nso xmusic.elf xmusic.nso
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}  NSO creation failed!${NC}"
        cd ..
        return 1
    fi
    
    echo -e "${GREEN}  ✅ Sysmodule built successfully!${NC}"
    cd ..
    return 0
}

# Function to build overlay
build_overlay() {
    echo -e "${YELLOW}Building overlay...${NC}"
    
    # Check if libtesla exists
    if [ ! -d "libs/libtesla" ]; then
        echo "  Installing libtesla..."
        mkdir -p libs
        cd libs
        git clone https://github.com/WerWolv/libtesla.git
        cd libtesla
        make
        cd ../..
    fi
    
    cd overlay
    rm -rf build *.elf *.ovl
    mkdir -p build
    
    # Compile main
    echo "  Compiling overlay..."
    aarch64-none-elf-g++ \
        -g -Wall -O2 -ffunction-sections \
        -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE \
        -I../common -I$DEVKITPRO/libnx/include -I../libs/libtesla/include \
        -D__SWITCH__ -DTESLA_INIT_IMPL -std=gnu++17 -fno-rtti -fno-exceptions \
        -c source/main.cpp -o build/main.o
    
    if [ $? -ne 0 ]; then
        echo -e "${YELLOW}  Overlay compilation failed (this is normal if libtesla is missing)${NC}"
        cd ..
        return 1
    fi
    
    # Link
    echo "  Linking overlay..."
    aarch64-none-elf-g++ \
        -specs=$DEVKITPRO/libnx/switch.specs \
        -g -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE \
        build/main.o \
        -L$DEVKITPRO/libnx/lib -L../libs/libtesla \
        -ltesla -lnx \
        -o xmusic-overlay.elf
    
    # Convert to OVL
    echo "  Creating OVL..."
    elf2nso xmusic-overlay.elf xmusic-overlay.ovl 2>/dev/null || \
    cp xmusic-overlay.elf xmusic-overlay.ovl
    
    echo -e "${GREEN}  ✅ Overlay built!${NC}"
    cd ..
    return 0
}

# Create distribution
create_dist() {
    echo -e "${YELLOW}Creating distribution package...${NC}"
    
    rm -rf dist
    mkdir -p dist/atmosphere/contents/58000000000000A1/flags
    mkdir -p dist/switch/.overlays
    mkdir -p dist/config/xmusic
    
    # Copy sysmodule
    if [ -f "sysmodule/xmusic.nso" ]; then
        cp sysmodule/xmusic.nso dist/atmosphere/contents/58000000000000A1/exefs.nso
        echo -e "${GREEN}  ✅ Sysmodule copied${NC}"
    fi
    
    # Copy overlay
    if [ -f "overlay/xmusic-overlay.ovl" ]; then
        cp overlay/xmusic-overlay.ovl dist/switch/.overlays/
        echo -e "${GREEN}  ✅ Overlay copied${NC}"
    fi
    
    # Create boot flag
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
    
    # Create config
    cat > dist/config/xmusic/config.json << 'JSON_END'
{
    "default_volume": 0.3,
    "auto_play": false,
    "startup_sound": true,
    "api_endpoint": "https://invidious.fdn.fr"
}
JSON_END
    
    # Create info file
    cat > dist/README.txt << 'TXT_END'
XMusic v0.1.0-alpha
===================

Installation:
1. Copy all folders to SD card root
2. Restart your Switch

Usage:
- Press L + Dpad Down + R3 to open Tesla Menu
- Select XMusic
- Use the controls to play/pause and adjust volume

The sysmodule will play a startup sound when loaded.

GitHub: https://github.com/yourusername/XMusic
TXT_END
    
    echo -e "${GREEN}✅ Distribution package created!${NC}"
}

# Main build process
echo ""
echo "Step 1: Building sysmodule..."
build_sysmodule
SYSMODULE_RESULT=$?

echo ""
echo "Step 2: Building overlay..."
build_overlay
OVERLAY_RESULT=$?

echo ""
echo "Step 3: Creating distribution..."
create_dist

# Summary
echo ""
echo "======================================"
echo "          Build Summary               "
echo "======================================"

if [ $SYSMODULE_RESULT -eq 0 ]; then
    echo -e "${GREEN}✅ Sysmodule: SUCCESS${NC}"
    ls -lh sysmodule/xmusic.nso 2>/dev/null
else
    echo -e "${RED}❌ Sysmodule: FAILED${NC}"
fi

if [ $OVERLAY_RESULT -eq 0 ]; then
    echo -e "${GREEN}✅ Overlay: SUCCESS${NC}"
    ls -lh overlay/xmusic-overlay.ovl 2>/dev/null
else
    echo -e "${YELLOW}⚠️  Overlay: SKIPPED${NC}"
fi

echo ""
echo "Distribution files:"
ls -la dist/atmosphere/contents/58000000000000A1/ 2>/dev/null
echo ""

if [ $SYSMODULE_RESULT -eq 0 ]; then
    echo -e "${GREEN}======================================"
    echo "    🎉 XMusic Build Complete! 🎉     "
    echo "======================================${NC}"
    echo ""
    echo "To install on your Switch:"
    echo "1. Copy everything in 'dist/' to SD card root"
    echo "2. Restart your Switch"
    echo "3. You should hear a startup melody!"
    echo "4. Open Tesla (L + Dpad Down + R3) > XMusic"
else
    echo -e "${RED}Build failed. Check errors above.${NC}"
fi