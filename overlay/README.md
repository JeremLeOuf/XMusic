# 🎮 XMusic Controller

A graphical controller application for interacting with the XMusic background service.

## Features

- **Interactive Controls** - Use gamepad buttons to control playback
- **Real-time Status** - Display current playing status and volume
- **Service Integration** - Direct communication with XMusic sysmodule
- **Console Interface** - Easy-to-use text-based UI

## Controls

| Button | Action |
|--------|--------|
| **A** | Play/Pause toggle |
| **X** | Next Track |
| **Y** | Previous Track |
| **L** | Volume Down |
| **R** | Volume Up |
| **ZL** | Get Status |
| **+** | Exit |

## Installation

1. **Build the controller:**
   ```bash
   cd overlay
   ./build_controller.sh
   ```

2. **Copy to Switch:**
   ```bash
   # Copy to Switch SD card
   cp XMusicController.nro /path/to/switch/sd/switch/
   ```

3. **Run on Switch:**
   - Ensure XMusic sysmodule is installed and running
   - Launch Homebrew Launcher
   - Run "XMusicController"

## Usage

### Starting the Controller
1. Launch from Homebrew Launcher
2. You'll see the connection status to XMusic service
3. If connected, use the gamepad controls to interact

### Status Display
Press **ZL** to see current status:
```
📊 XMusic Status:
   Playing: Yes
   Volume: 30%
   Title: [track title]
   Artist: [artist name]
   Position: 45/180s
```

### Controls
- **Play/Pause**: Press **A** to toggle playback
- **Navigation**: Use **X** (next) and **Y** (previous) for track control
- **Volume**: Use **L** and **R** for volume adjustment
- **Exit**: Press **+** to close the controller

## Troubleshooting

### "Service Not Available"
- Ensure XMusic sysmodule is installed in `/atmosphere/contents/58000000000000A1/`
- Restart your Switch to load the sysmodule
- Check that you hear the startup melody when booting

### Controls Not Working
- Make sure you're pressing the correct buttons (see control table above)
- Check that the controller is properly connected
- Try getting status first with **ZL** to verify service communication

### No Audio Response
- The sysmodule handles audio, not the controller
- Check that the sysmodule is working by listening for startup melody
- Use the test client (`test_client.nro`) to verify IPC communication

## Technical Details

### Architecture
- **Frontend**: Console-based interface using libnx
- **Communication**: IPC service calls to XMusic sysmodule
- **Input**: Modern libnx pad input handling
- **Threading**: Single-threaded with responsive input polling

### Build Requirements
- DevkitPro with devkitA64
- libnx framework
- Standard C++17 compiler

### File Size
- **Executable**: ~740KB NRO file
- **Memory**: Minimal RAM usage (~2MB)
- **Performance**: Responsive input with no lag

## Development

### Source Structure
```
overlay/
├── source/
│   └── main.cpp           # Controller implementation
├── build_controller.sh    # Build script
└── Makefile              # Alternative build system
```

### Key Classes
- **XMusicController**: IPC service communication
- **Input Handling**: Modern pad input with button mapping
- **Status Display**: Real-time service status formatting

### Future Improvements
- [ ] GUI overlay using Tesla framework
- [ ] Visual volume indicator
- [ ] Track progress bar
- [ ] Playlist display
- [ ] Search functionality

This controller provides a bridge between user input and the XMusic background service, enabling full control over audio playback without interrupting games or other applications.
