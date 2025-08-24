#!/bin/bash

# XMusic Build Script

echo "======================================"
echo "     XMusic v0.1.0-alpha Builder     "
echo "======================================"

# Check for devkitPro
if [ -z "$DEVKITPRO" ]; then
    echo "Error: DEVKITPRO not set. Please install devkitPro first."
    echo "Visit: https://devkitpro.org/wiki/Getting_Started"
    exit 1
fi

# Check if project is set up
if [ ! -f "sysmodule/Makefile" ]; then
    echo "Project not initialized. Running setup..."
    if [ -f "setup_project.sh" ]; then
        chmod +x setup_project.sh
        ./setup_project.sh
    else
        echo "Error: setup_project.sh not found!"
        exit 1
    fi
fi

# Clean previous builds
echo ""
echo "Step 1: Cleaning previous builds..."
make -C sysmodule clean 2>/dev/null
make -C overlay clean 2>/dev/null

# Build sysmodule
echo ""
echo "Step 2: Building sysmodule..."
make -C sysmodule

if [ $? -ne 0 ]; then
    echo "❌ Sysmodule build failed!"
    exit 1
fi

# Build overlay (simplified for now)
echo ""
echo "Step 3: Building overlay..."
make -C overlay

if [ $? -ne 0 ]; then
    echo "⚠️  Overlay build failed (this is normal for now)"
    echo "Continuing with sysmodule only..."
fi

# Create distribution folder
echo ""
echo "Step 4: Creating distribution package..."
rm -rf dist
mkdir -p dist/atmosphere/contents/58000000000000A1/flags
mkdir -p dist/switch/.overlays
mkdir -p dist/config/xmusic

# Copy files
if [ -f "sysmodule/xmusic.nso" ]; then
    cp sysmodule/xmusic.nso dist/atmosphere/contents/58000000000000A1/exefs.nso
    echo "✅ Sysmodule copied"
else
    echo "⚠️  Sysmodule not found"
fi

if [ -f "overlay/xmusic-overlay.ovl" ]; then
    cp overlay/xmusic-overlay.ovl dist/switch/.overlays/
    echo "✅ Overlay copied"
fi

if [ -f "atmosphere/contents/58000000000000A1/toolbox.json" ]; then
    cp atmosphere/contents/58000000000000A1/toolbox.json dist/atmosphere/contents/58000000000000A1/
fi

# Create boot flag
touch dist/atmosphere/contents/58000000000000A1/flags/boot2.flag

# Create config
cat > dist/config/xmusic/config.json << EOF
{
    "api_endpoint": "https://invidious.fdn.fr",
    "default_volume": 0.3,
    "auto_play": false
}
EOF

echo ""
echo "======================================"
echo "✅ Build complete!"
echo "======================================"
echo ""
echo "Files created:"
ls -la dist/atmosphere/contents/58000000000000A1/ 2>/dev/null
echo ""
echo "To install:"
echo "1. Copy everything in 'dist/' to your SD card root"
echo "2. Restart your Switch"
echo ""
echo "This is a test build - service will run but has limited functionality"

# Build sysmodule
echo "Building sysmodule..."
make -C sysmodule clean
make -C sysmodule

if [ $? -ne 0 ]; then
    echo "Sysmodule build failed!"
    exit 1
fi

# Build overlay
echo "Building overlay..."
make -C overlay clean
make -C overlay

if [ $? -ne 0 ]; then
    echo "Overlay build failed!"
    exit 1
fi

# Create distribution folder
echo "Creating distribution package..."
mkdir -p dist/atmosphere/contents/420000000000BEEF/flags
mkdir -p dist/switch/.overlays
mkdir -p dist/config/music-overlay

# Copy files
cp sysmodule/music-sysmodule.nsp dist/atmosphere/contents/420000000000BEEF/exefs.nsp
cp overlay/music-overlay.ovl dist/switch/.overlays/
cp atmosphere/contents/420000000000BEEF/toolbox.json dist/atmosphere/contents/420000000000BEEF/

# Create empty flag file to auto-start
touch dist/atmosphere/contents/420000000000BEEF/flags/boot2.flag

# Create default config
cat > dist/config/music-overlay/config.json << EOF
{
    "invidious_instance": "https://invidious.fdn.fr",
    "default_volume": 0.5,
    "auto_play": false
}
EOF

echo "Build complete! Files are in dist/"
echo ""
echo "Installation:"
echo "1. Copy the contents of 'dist/' to your SD card root"
echo "2. Restart your Switch"
echo "3. Open Tesla Menu (L + Dpad Down + R3)"
echo "4. Select Music Overlay"
