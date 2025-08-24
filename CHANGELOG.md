# Changelog

All notable changes to XMusic will be documented in this file.

## [0.1.0-alpha] - 2025-08-24

### Added
- Initial working release
- Background audio service (sysmodule)
- Startup melody playback (Mario coin style)
- Test tone generation (440Hz sine wave)
- Service registration and background loop
- Low memory footprint (~2MB)
- Basic audio manager with PCM playback
- Proper build system with single working script

### Technical Details
- Uses modern libnx API
- Simplified IPC implementation
- Threaded audio playback
- Memory-aligned audio buffers
- Stereo audio output support

### Known Limitations
- No streaming functionality yet
- No user interface (overlay)
- No external controls (play/pause)
- Limited to test audio only

## [Unreleased/Planned]

### v0.2.0 - IPC & Controls
- [ ] Fix IPC command handling
- [ ] Tesla overlay interface
- [ ] Basic play/pause controls
- [ ] Volume adjustment
- [ ] Status reporting

### v0.3.0 - Audio Streaming  
- [ ] YouTube Music integration
- [ ] Network streaming capability
- [ ] Audio format decoding
- [ ] Buffer management

### v0.4.0 - User Interface
- [ ] Search functionality
- [ ] Track navigation
- [ ] Playlist support
- [ ] Settings persistence

### v1.0.0 - Full Release
- [ ] Multiple streaming services
- [ ] Local file playback
- [ ] Advanced features (equalizer, etc.)
- [ ] Performance optimizations

## Development Notes

### Build System
- Cleaned up from 12+ build scripts to 1 working script
- Removed broken/obsolete source files
- Simplified project structure
- Focus on core functionality first

### Code Quality
- Fixed compilation issues with modern libnx
- Removed complex features until basics work
- Added proper error handling
- Memory leak prevention

### Future Planning
- Modular architecture for easy feature addition
- Clear separation between audio engine and UI
- Extensible streaming backend
- Performance monitoring capabilities