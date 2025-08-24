# 🎯 XMusic - Development Roadmap

## ✅ Milestone 1: Core Foundation (COMPLETED)
- [x] Project structure cleanup
- [x] Working build system (single script)
- [x] Sysmodule compilation 
- [x] Audio manager implementation
- [x] Background service loop
- [x] Startup melody playback
- [x] Test tone generation
- [x] Memory management
- [x] Service registration
- [x] Basic documentation

**Status**: ✅ **WORKING** - Builds successfully, plays audio, runs stably

## 🔧 Milestone 2: IPC Communication (CURRENT FOCUS)

The audio engine works, but we need proper IPC to control it.

### Priority Tasks:
1. **Fix IPC API Issues** 
   - Research modern libnx IPC patterns
   - Replace broken `IpcParsedCommand` usage
   - Implement simple command handling

2. **Basic Commands**
   ```cpp
   // Target IPC commands
   XMusicCmd_Play     // Start playback
   XMusicCmd_Pause    // Stop playback  
   XMusicCmd_GetStatus // Return current state
   XMusicCmd_SetVolume // Adjust volume
   ```

3. **Testing Method**
   ```bash
   # Test service communication
   # Via homebrew test app or Tesla overlay
   ```

## 🎮 Milestone 3: Tesla Overlay Interface

Once IPC works, create user controls.

### Step 1: Minimal Overlay
```cpp
// Simple Tesla menu with:
// - Play/Pause button
// - Volume slider  
// - Status display
```

### Step 2: Control Integration
```cpp
// Connect overlay to service
// Test all commands work
// Handle connection errors
```

## � Milestone 4: Network Streaming

Add internet music capability.

### Step 1: HTTP Client
```cpp
class SimpleHttpClient {
    std::string get(const std::string& url);
    // Basic GET requests only
};
```

### Step 2: Audio Streaming
```cpp
class StreamingPlayer {
    // Network→buffer→audio pipeline
    // Handle buffering/dropouts
};
```

### Step 3: YouTube Integration
```cpp
// Use Invidious API for track search
// Extract audio stream URLs
// Handle format conversion
```

## 📋 Current TODO (Priority Order)

### Week 1: IPC Fix
- [ ] Research libnx IPC documentation
- [ ] Create minimal IPC test program  
- [ ] Fix service command handling
- [ ] Test play/pause functionality

### Week 2: Basic UI
- [ ] Tesla overlay template
- [ ] Connect to XMusic service
- [ ] Basic controls (play/pause)
- [ ] Status display

### Week 3: Audio Controls
- [ ] Volume adjustment working
- [ ] Track progress/duration
- [ ] Multiple audio track support
- [ ] Memory optimization

### Week 4: Streaming Foundation
- [ ] HTTP client implementation
- [ ] Basic streaming test
- [ ] Audio format research
- [ ] Buffer management

## 🧪 Testing Checklist

### Hardware Testing:
- [ ] Boots without crashes
- [ ] Plays startup melody
- [ ] Runs continuously
- [ ] Memory usage stable
- [ ] No interference with games

### IPC Testing:
- [ ] Service accepts connections
- [ ] Commands execute correctly
- [ ] Error handling works
- [ ] No memory leaks

### Audio Testing:
- [ ] Clear sound output
- [ ] Volume control works
- [ ] No audio glitches
- [ ] Proper mixing

## 🛠️ Development Commands

```bash
# Build and test
./build.sh

# Install on Switch
cp -r dist/* /path/to/sd/

# Check service status  
# (via homebrew app to query service)

# Debug memory
# Monitor via Atmosphère tools
```

## � Research Needed

1. **Modern libnx IPC**: Find working examples of service communication
2. **Tesla Integration**: Study existing overlays for patterns
3. **Audio Streaming**: Research Switch audio format requirements
4. **Memory Limits**: Test maximum concurrent audio buffer size

## � Implementation Notes

### IPC Service Pattern:
```cpp
// Target pattern (needs research):
void handleIPCRequest() {
    // Parse command from client
    // Execute action
    // Send response
}
```

### Audio Architecture:
```cpp
// Current working pattern:
AudioManager → PCM Buffer → Switch Audio Output
// Future: Network → Decoder → AudioManager → Output
```

## � Known Issues to Address

1. **IPC API Mismatch**: Old API calls don't exist in current libnx
2. **No Error Handling**: Service doesn't handle failed commands
3. **Static Audio**: Only plays predefined sounds
4. **No Persistence**: Settings don't save between reboots

## 📊 Progress Tracking

- **Core Service**: ✅ 100% Working
- **Audio Engine**: ✅ 100% Working  
- **IPC System**: ❌ 0% Broken
- **User Interface**: ❌ 0% Not started
- **Streaming**: ❌ 0% Future feature

**Next Milestone**: Get IPC working for basic play/pause control.

---

**Current Status**: Basic audio sysmodule working perfectly. Next step is fixing IPC communication to enable external control.