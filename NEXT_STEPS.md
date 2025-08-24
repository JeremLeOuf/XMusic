# 🎯 XMusic - Next Development Steps

## ✅ Milestone 1: Basic Service (COMPLETED)
- [x] Project structure
- [x] Compile sysmodule
- [x] Create NSO file
- [x] Basic service loop

## 🔧 Milestone 2: Audio Foundation (CURRENT)
Next, we need to add basic audio playback:

### Step 1: Test Audio Output
Create a simple test that plays a beep/tone to verify audio works.

```cpp
// Add to main.cpp
#include <switch/audio/audio.h>

void testAudioBeep() {
    // Generate 440Hz tone (A4 note)
    // Play for 1 second
}
```

### Step 2: IPC Service Implementation
Make the service actually respond to commands:

```cpp
// Implement service handler
void handleIPCRequest() {
    // Handle Play/Pause/Stop commands
}
```

### Step 3: Memory Audio Player
Play audio from memory (no streaming yet):

```cpp
class SimpleAudioPlayer {
    // PCM buffer playback
    // Volume control
    // Basic mixing
};
```

## 🎵 Milestone 3: Streaming Integration

### Step 1: HTTP Client
```cpp
// Basic curl wrapper
class HttpClient {
    std::string get(const std::string& url);
    std::string post(const std::string& url, const std::string& data);
};
```

### Step 2: YouTube/Invidious API
```cpp
class YouTubeClient {
    std::vector<Track> search(const std::string& query);
    std::string getStreamUrl(const std::string& videoId);
};
```

### Step 3: Audio Streaming
```cpp
class StreamingAudioPlayer {
    // Buffer management
    // Network streaming
    // Format conversion
};
```

## 🎮 Milestone 4: Tesla Overlay

### Step 1: Basic Overlay
- Install libtesla
- Create basic UI
- Connect to service

### Step 2: Controls
- Play/Pause button
- Volume slider
- Track info display

### Step 3: Search Interface
- Text input
- Results list
- Quick presets

## 📊 Testing Checklist

### On Real Hardware:
- [ ] Service starts on boot
- [ ] No crashes/freezes
- [ ] Memory usage < 4MB
- [ ] Audio plays without glitches
- [ ] Works while gaming

### Performance:
- [ ] CPU usage < 5%
- [ ] No game slowdown
- [ ] Smooth audio playback
- [ ] Fast search results

## 🛠️ Development Commands

```bash
# Quick rebuild
./simple_build.sh

# Test on Switch
# 1. Copy to SD: cp -r dist/* /path/to/sd/
# 2. Check logs: cat /atmosphere/contents/58000000000000A1/logs/xmusic.log

# Debug mode build
./simple_build.sh --debug
```

## 📝 Current TODO

1. **IMMEDIATE**: Test on real Switch hardware
2. **TODAY**: Add simple audio test (beep/tone)
3. **THIS WEEK**: Implement IPC commands
4. **NEXT WEEK**: Add streaming capability

## 🐛 Known Issues to Fix

- [ ] No actual audio playback yet
- [ ] Service doesn't handle IPC
- [ ] No overlay interface
- [ ] No network functionality

## 💡 Ideas for v0.2.0

- SoundCloud support
- Local file playback
- Playlist management
- Sleep timer
- Equalizer
- Discord Rich Presence

## 📚 Helpful Resources

- [libnx audio docs](https://switchbrew.github.io/libnx/)
- [Tesla overlay examples](https://github.com/WerWolv/Tesla-Menu)
- [Atmosphère sysmodule guide](https://github.com/Atmosphere-NX/Atmosphere/wiki)
- [Switch homebrew Discord](https://discord.gg/switch)

---

**Next immediate action**: Test the compiled sysmodule on your Switch!