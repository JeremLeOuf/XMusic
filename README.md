# 🎵 XMusic - Background Audio Player for Nintendo Switch

[![License](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)
[![Switch](https://img.shields.io/badge/Nintendo-Switch-red.svg)](https://github.com/switchbrew/libnx)
[![Version](https://img.shields.io/badge/Version-0.1.0--alpha-yellow.svg)](https://github.com/JeremLeOuf/XMusic/releases)

XMusic is a lightweight homebrew sysmodule that provides background audio playback for Nintendo Switch.

## ✨ Current Features

- 🎮 **Background audio service** - Runs independently of games
- 🎵 **Startup melody** - Mario-style coin sound on boot
- 🔊 **Test tone generation** - 440Hz sine wave for testing
- 💾 **Low memory footprint** - ~2MB RAM usage
- ⚡ **Fast startup** - Begins playback within seconds
- �️ **Stable operation** - Designed for 24/7 runtime

## 📋 Requirements

- Nintendo Switch with CFW (Atmosphère 1.0.0+)
- 2MB+ free memory
- Audio output capability

## 🚀 Installation

1. Download the latest release from [Releases](https://github.com/JeremLeOuf/XMusic/releases)
2. Extract to SD card root
3. Restart your Switch
4. Listen for startup melody!

## 🔧 Building from Source

### Prerequisites
```bash
# Install devkitPro
wget https://apt.devkitpro.org/install-devkitpro-pacman
chmod +x ./install-devkitpro-pacman
./install-devkitpro-pacman

# Install Switch tools
sudo dkp-pacman -S switch-dev
```

### Build
```bash
git clone https://github.com/JeremLeOuf/XMusic.git
cd XMusic
./build.sh
```

### Install
```bash
# Copy to SD card
cp -r dist/* /path/to/sd/card/
```

## 🏗️ Project Structure

```
XMusic/
├── build.sh              # Main build script
├── sysmodule/            # Core service
│   └── source/
│       ├── main.cpp              # Service entry point
│       └── audio_manager.h       # Audio engine
├── overlay/              # UI (future)
├── common/               # Shared headers
│   └── xmusic_ipc.h     # IPC definitions
└── dist/                 # Built files
```

## 🎵 How It Works

1. **Service Registration**: Creates `xmusic` service on boot
2. **Audio Initialization**: Sets up audio output pipeline
3. **Melody Playback**: Plays startup sound for 2 seconds
4. **Background Loop**: Continues running for future features

## 🗺️ Roadmap

### v0.2.0 - IPC Controls
- [ ] Play/Pause commands
- [ ] Volume control
- [ ] Status reporting
- [ ] Tesla overlay interface

### v0.3.0 - Audio Streaming
- [ ] YouTube Music integration
- [ ] SoundCloud support
- [ ] Network streaming
- [ ] Audio format support

### v1.0.0 - Full Features
- [ ] Playlist management
- [ ] Local file playback
- [ ] Search functionality
- [ ] Settings persistence

## ⚠️ Current Limitations

- **Audio only**: No streaming or controls yet
- **Test sounds**: Only plays startup melody and test tone
- **No UI**: Command interface not implemented
- **No networking**: Streaming features planned for future

## � Testing

The sysmodule will:
1. Play a 4-note startup melody (Mario coin style)
2. Generate a 440Hz test tone
3. Run continuously in background
4. Use ~2MB RAM
5. Register `xmusic` service

## 🛠️ Development

### Adding Features
```cpp
// Example: Add new audio pattern
void AudioManager::loadCustomPattern() {
    // Generate audio samples
    // Load into playback buffer
}
```

### Debug Mode
```bash
# Build with debug symbols
./build.sh --debug

# Check logs
tail -f /atmosphere/crash_reports/
```

## � Performance

- **Memory**: ~2MB RAM usage
- **CPU**: <1% background usage  
- **Audio latency**: <50ms
- **Boot time**: 2-3 seconds to melody

## 🤝 Contributing

1. Fork the repository
2. Create feature branch
3. Test on real hardware
4. Submit pull request

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

## 📝 License

GPLv3 - See [LICENSE](LICENSE) file for details.

## 🏆 Credits

- **Atmosphère Team** - CFW framework
- **devkitPro** - Development tools
- **libnx** - Switch libraries
- **Switch homebrew community** - Support and feedback

---

**Status**: Alpha - Basic audio playback working, streaming features in development.

For issues and feature requests, please use [GitHub Issues](https://github.com/JeremLeOuf/XMusic/issues).