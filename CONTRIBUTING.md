# Contributing to XMusic

First off, thank you for considering contributing to XMusic! 🎉

## 🤝 How to Contribute

### Reporting Bugs

Before creating bug reports, please check existing issues to avoid duplicates.

**When reporting bugs, include:**
- Your Switch firmware version
- Atmosphère version
- XMusic version
- Steps to reproduce
- Expected vs actual behavior
- Crash logs if available (from `/atmosphere/crash_reports/`)

### Suggesting Features

Open an issue with the `enhancement` label and describe:
- The feature you'd like
- Why it would be useful
- How it might work

### Pull Requests

1. Fork the repo
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📝 Development Setup

### Prerequisites

```bash
# Install devkitPro
wget https://apt.devkitpro.org/install-devkitpro-pacman
chmod +x ./install-devkitpro-pacman
./install-devkitpro-pacman

# Install Switch development tools
sudo dkp-pacman -S switch-dev switch-curl
```

### Building

```bash
git clone https://github.com/yourusername/XMusic.git
cd XMusic
./build.sh
```

### Testing

1. Copy `dist/` contents to SD card
2. Enable debug logging in config
3. Check logs at `/atmosphere/contents/58000000000000A1/logs/`

## 🎨 Code Style

- Use 4 spaces for indentation
- Follow existing code patterns
- Comment complex logic
- Keep functions small and focused

### Example:

```cpp
// Good
void XMusic::playTrack(const std::string& trackId) {
    // Stop current playback
    stopPlayback();
    
    // Load new track
    if (loadTrack(trackId)) {
        startPlayback();
        updateStatus();
    }
}
```

## 📦 Project Structure

```
XMusic/
├── sysmodule/      # Background service code
├── overlay/        # Tesla UI code
├── common/         # Shared headers
└── docs/           # Documentation
```

### Key Files:

- `sysmodule/source/main.cpp` - Service entry point
- `overlay/source/xmusic_gui.cpp` - UI implementation
- `common/xmusic_ipc.h` - IPC protocol definition

## 🧪 Testing Guidelines

### Before submitting PR:

- [ ] Code compiles without warnings
- [ ] Tested on real hardware
- [ ] No memory leaks (check with debug tools)
- [ ] UI remains responsive
- [ ] Audio doesn't glitch

### Test scenarios:

1. Play music → Launch game → Music continues
2. Change volume while game is running
3. Search for tracks with special characters
4. Network disconnection handling
5. Sleep mode → Wake → Music resumes

## 📚 Documentation

- Update README.md if adding features
- Document new IPC commands
- Add comments for complex algorithms
- Update version numbers

## 🚀 Release Process

1. Update version in `toolbox.json`
2. Update CHANGELOG.md
3. Create release build
4. Test on clean system
5. Create GitHub release

## 💬 Communication

- **Discord**: Best for quick questions
- **GitHub Issues**: Bug reports and features
- **Pull Requests**: Code contributions

## ⚖️ License

By contributing, you agree that your contributions will be licensed under GPLv3.

## 🙏 Recognition

Contributors will be added to:
- README.md credits section
- GitHub contributors page
- Discord special role

Thank you for helping make XMusic better! 🎵