# 🎵 XMusic - Background Music Player for Nintendo Switch

[![License](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENSE)
[![Switch](https://img.shields.io/badge/Nintendo-Switch-red.svg)](https://github.com/switchbrew/libnx)
[![Homebrew](https://img.shields.io/badge/Homebrew-CFW-orange.svg)](https://github.com/Atmosphere-NX/Atmosphere)
[![Version](https://img.shields.io/badge/Version-0.1.0--alpha-yellow.svg)](https://github.com/yourusername/XMusic/releases)

XMusic is a homebrew sysmodule that brings background music streaming to your Nintendo Switch. Play YouTube Music and SoundCloud tracks while gaming!

<p align="center">
  <img src="docs/screenshot.png" alt="XMusic Screenshot" width="600">
</p>

## ✨ Features

- 🎮 **Play music while gaming** - Runs as a background service
- 🎵 **YouTube Music integration** - Search and stream tracks
- ☁️ **SoundCloud support** - Access to millions of tracks
- 🎚️ **Volume mixing** - Adjustable volume that doesn't override game audio
- 🎯 **Tesla Overlay interface** - Quick access via L + Dpad Down + R3
- 🔄 **Playlist management** - Queue, skip, and repeat tracks
- 💾 **Low memory footprint** - Optimized for Switch hardware

## 📋 Requirements

- Nintendo Switch with CFW (Atmosphère 1.0.0+)
- Tesla Menu installed
- Internet connection for streaming

## 🚀 Installation

### Quick Install

1. Download the latest release from [Releases](https://github.com/yourusername/XMusic/releases)
2. Extract `XMusic.zip` to the root of your SD card
3. Reboot your Switch

### Manual Build

```bash
git clone https://github.com/yourusername/XMusic.git
cd XMusic
chmod +x build.sh
./build.sh
```

Copy the `dist/` folder contents to your SD card.

## 🎮 Usage

1. **Open XMusic**: Press `L + Dpad Down + R3` to open Tesla Menu, select XMusic
2. **Search music**: Select "Search" and enter artist/song name
3. **Quick play**: Choose from preset playlists (Lofi, Gaming, etc.)
4. **Controls**:
   - `A` - Select/Play
   - `B` - Back
   - `X` - Pause/Resume
   - `Y` - Skip track
   - `L/R` - Volume control

## 🏗️ Project Structure

```
XMusic/
├── sysmodule/          # Background service
│   └── source/
│       ├── main.cpp            # Service entry point
│       ├── audio_player.cpp    # Audio streaming engine
│       └── streaming/          # YouTube/SoundCloud clients
├── overlay/            # Tesla UI
│   └── source/
│       └── xmusic_gui.cpp     # User interface
└── common/             # Shared definitions
    └── xmusic_ipc.h    # IPC protocol
```

## 🔧 Configuration

Edit `/config/xmusic/settings.json`:

```json
{
  "default_volume": 0.3,
  "auto_play": false,
  "streaming_quality": "medium",
  "cache_size_mb": 50
}
```

## 🛠️ Development

### Prerequisites

- [devkitPro](https://devkitpro.org/wiki/Getting_Started)
- switch-dev, switch-curl packages
- [libtesla](https://github.com/WerWolv/libtesla)

### Building from source

```bash
# Install dependencies
sudo dkp-pacman -S switch-dev switch-curl

# Clone and build
git clone https://github.com/yourusername/XMusic.git
cd XMusic
make
```

### Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## 🗺️ Roadmap

- [x] Basic audio playback
- [x] Tesla overlay interface
- [x] YouTube Music search
- [ ] SoundCloud integration
- [ ] Spotify support (via proxy)
- [ ] Local file playback
- [ ] Equalizer settings
- [ ] Discord Rich Presence
- [ ] Lyrics display
- [ ] Sleep timer

## ⚠️ Disclaimer

This project is for educational purposes. XMusic does not download or store copyrighted content. It acts as a streaming client similar to a web browser. Please respect copyright laws in your jurisdiction.

## 🤝 Credits

- **[Atmosphère](https://github.com/Atmosphere-NX/Atmosphere)** - CFW framework
- **[libnx](https://github.com/switchbrew/libnx)** - Switch development library
- **[Tesla](https://github.com/WerWolv/Tesla-Menu)** - Overlay system
- **[Invidious](https://invidious.io/)** - YouTube proxy API

## 📝 License

This project is licensed under GPLv3 - see [LICENSE](LICENSE) file for details.

## 💬 Community

- **Discord**: [Join our server](https://discord.gg/xmusic)
- **Reddit**: [r/SwitchHacks](https://reddit.com/r/SwitchHacks)
- **GBAtemp**: [XMusic Thread](https://gbatemp.net/)

## 🐛 Known Issues

- Audio may stutter on poor network connections
- Some YouTube videos may not stream (region/age restricted)
- Volume changes may take a second to apply

## 📧 Contact

- **GitHub Issues**: [Report bugs](https://github.com/yourusername/XMusic/issues)
- **Email**: xmusic.switch@example.com

---

<p align="center">
Made with ❤️ for the Switch homebrew community
</p>

<p align="center">
  <a href="https://github.com/yourusername/XMusic/stargazers">⭐ Star this project if you find it useful!</a>
</p>