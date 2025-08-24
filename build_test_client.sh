#!/bin/bash

# XMusic Test Client Build Script

export DEVKITPRO=/opt/devkitpro
export DEVKITARM=$DEVKITPRO/devkitARM  
export DEVKITA64=$DEVKITPRO/devkitA64
export PATH=$DEVKITPRO/tools/bin:$DEVKITA64/bin:$PATH

echo "======================================="
echo "    XMusic Test Client Build          "
echo "======================================="

# Clean previous build
echo "Cleaning previous build..."
rm -f test_client.elf test_client.nro

# Compile test client
echo "Compiling test client..."
aarch64-none-elf-g++ \
    -MMD -MP -MF test_client.d \
    -g -Wall -O2 -ffunction-sections \
    -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE \
    -I$DEVKITPRO/libnx/include \
    -I$DEVKITPRO/portlibs/switch/include \
    -I. \
    -D__SWITCH__ \
    -c test_client.cpp -o test_client.o

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
    test_client.o \
    -L$DEVKITPRO/libnx/lib \
    -L$DEVKITPRO/portlibs/switch/lib \
    -lnx \
    -lm \
    -o test_client.elf

if [ $? -ne 0 ]; then
    echo "❌ Linking failed!"
    exit 1
fi

# Convert to NRO for homebrew testing
echo "Creating NRO..."
elf2nro test_client.elf test_client.nro

if [ $? -ne 0 ]; then
    echo "❌ NRO creation failed!"
    exit 1
fi

echo ""
echo "======================================="
echo "   ✅ Test Client Build Success!      "
echo "======================================="
echo ""
echo "Output files:"
ls -la test_client.elf test_client.nro 2>/dev/null
echo ""
echo "To test:"
echo "  1. Install XMusic sysmodule first"
echo "  2. Copy test_client.nro to Switch SD"
echo "  3. Run via Homebrew Launcher"
echo "  4. Check console output for results"
