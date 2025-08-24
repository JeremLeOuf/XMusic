#!/bin/bash

echo "======================================"
echo "   Preparing XMusic for Release       "
echo "======================================"

VERSION="v0.1.0-alpha"

# Build everything
echo "Building XMusic..."
chmod +x build_complete.sh
./build_complete.sh

if [ ! -f "dist/atmosphere/contents/58000000000000A1/exefs.nso" ]; then
    echo "Build failed!"
    exit 1
fi

# Create release zip
echo "Creating release package..."
cd dist
zip -r ../XMusic-${VERSION}.zip *
cd ..

# Initialize git if needed
if [ ! -d ".git" ]; then
    echo "Initializing git repository..."
    git init
    
    # Create .gitignore
    cat > .gitignore << 'GITIGNORE_END'
# Build files
build/
*.elf
*.nso
*.nsp
*.ovl
*.nacp

# Distribution
dist/
*.zip

# IDE
.vscode/
.idea/

# System
.DS_Store
Thumbs.db

# Logs
*.log

# Libraries (will be downloaded)
libs/libtesla/
GITIGNORE_END
fi

# Stage all files
echo "Staging files for commit..."
git add .

# Commit
echo "Creating commit..."
git commit -m "XMusic ${VERSION} - Background music player for Nintendo Switch

Features:
- Background audio playback
- Tesla overlay interface  
- Volume control
- Test audio (startup melody + tone)
- IPC service communication
- Low memory footprint (~4MB)

This is an alpha release for testing."

# Create tag
echo "Creating version tag..."
git tag -a ${VERSION} -m "XMusic ${VERSION} - First alpha release

- Basic audio playback working
- Tesla overlay for controls
- Startup sound plays on load
- Volume adjustment
- Play/Pause controls

Known issues:
- No streaming yet (only test audio)
- Limited features
- May need refinement

Test release for community feedback."

echo ""
echo "======================================"
echo "     📦 Release Package Ready!        "
echo "======================================"
echo ""
echo "Created: XMusic-${VERSION}.zip"
ls -lh XMusic-${VERSION}.zip
echo ""
echo "Next steps:"
echo ""
echo "1. Create GitHub repository:"
echo "   https://github.com/new"
echo ""
echo "2. Push to GitHub:"
echo "   git remote add origin https://github.com/YOURUSERNAME/XMusic.git"
echo "   git branch -M main"
echo "   git push -u origin main"
echo "   git push origin ${VERSION}"
echo ""
echo "3. Create release on GitHub:"
echo "   - Go to: https://github.com/YOURUSERNAME/XMusic/releases/new"
echo "   - Select tag: ${VERSION}"
echo "   - Upload: XMusic-${VERSION}.zip"
echo "   - Mark as pre-release"
echo ""
echo "4. Share on Reddit:"
echo "   - Post content is in: reddit_post.md"
echo "   - Add screenshot of Tesla overlay"
echo "   - Link to GitHub release"