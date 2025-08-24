# XMusic Testing Guide

## Current Status ✅

The XMusic project now has:
- **Working sysmodule** with audio engine and startup melody
- **Modern IPC service** for external control
- **Test client** to verify service communication
- **Clean project structure** (9 core files)

## Testing the IPC Service

### 1. Install the Sysmodule

```bash
# Build the sysmodule
./build.sh

# Copy to Switch SD card
cp -r dist/* /path/to/switch/sd/
```

The sysmodule will install to:
- `/atmosphere/contents/58000000000000A1/` (Title ID)
- Includes `exefs.nso` (main binary)
- Includes `toolbox.json` (metadata)

### 2. Build and Deploy Test Client

```bash
# Build the test client
./build_test_client.sh

# Copy to Switch homebrew folder
cp test_client.nro /path/to/switch/sd/switch/
```

### 3. Testing Process

1. **Restart Switch** - The sysmodule will start automatically
2. **Listen for startup melody** - You should hear a Mario coin-style melody
3. **Launch Homebrew Launcher**
4. **Run test_client.nro**
5. **Check console output** for IPC test results

### Expected Test Output

```
==================================
      XMusic IPC Test Client      
==================================
✅ Connected to XMusic service

Testing commands...
📊 Getting initial status...
✅ Status retrieved successfully
   Is Playing: No
   Volume: 1.0
   Title: 
   Artist: 

📻 Testing PLAY command...
✅ Command sent successfully: 0

⏸️  Testing PAUSE command...
✅ Command sent successfully: 1

🔊 Testing SET_VOLUME command...
✅ Command sent successfully: 6

📊 Getting final status...
✅ Status retrieved successfully

✅ Test completed successfully!
```

## Troubleshooting

### Service Not Found
If you get "Failed to connect to XMusic service":
1. Ensure the sysmodule is installed correctly
2. Restart the Switch to load the sysmodule
3. Check that the title ID matches: `58000000000000A1`

### No Audio
If you don't hear the startup melody:
1. Check that audio output is enabled on Switch
2. Verify the sysmodule loaded successfully
3. Audio plays through speakers/headphones connected to Switch

### Build Failures
If builds fail:
1. Ensure DevkitPro is installed: `/opt/devkitpro/`
2. Check that devkitA64 toolchain is available
3. Verify libnx is properly installed

## Next Development Steps

### Priority 1: Validate IPC ✅ (Current)
- [x] Create modern service implementation
- [x] Build test client for verification
- [ ] Test on real hardware
- [ ] Verify all command types work

### Priority 2: Tesla Overlay UI
- [ ] Create basic Tesla overlay
- [ ] Implement play/pause controls
- [ ] Add volume controls
- [ ] Status display

### Priority 3: Audio Features
- [ ] Add more audio generation types
- [ ] Implement playlist management
- [ ] Add track metadata support

### Priority 4: Streaming (Future)
- [ ] YouTube Music integration
- [ ] SoundCloud support
- [ ] Local file playback

## Technical Notes

### Service Architecture
- **Service Name**: `xmusic`
- **Title ID**: `58000000000000A1`
- **Commands**: Play, Pause, Next, Previous, GetStatus, Search, SetVolume, PlayUrl
- **Threading**: Service runs in background thread
- **Audio**: 48kHz stereo PCM output

### File Structure
```
XMusic/
├── sysmodule/source/
│   ├── main.cpp           (Service entry point)
│   ├── xmusic_service.h   (IPC service header)
│   ├── xmusic_service.cpp (IPC implementation)
│   └── audio_manager.h    (Audio engine)
├── common/
│   └── xmusic_ipc.h       (Shared IPC definitions)
├── build.sh               (Main build script)
├── test_client.cpp        (IPC test client)
└── build_test_client.sh   (Test client build)
```

### Build System
- Uses devkitA64 cross-compiler
- Links against libnx for Switch APIs
- Produces NSO for sysmodule, NRO for homebrew
- Clean build system with minimal dependencies
