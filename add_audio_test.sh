#!/bin/bash

echo "Adding audio test to XMusic..."

# Create new main.cpp with audio test
cat > sysmodule/source/main.cpp << 'CPP_END'
#include <switch.h>
#include <cstring>
#include <cstdio>
#include <cmath>

extern "C" {
    u32 __nx_applet_type = AppletType_None;
    u32 __nx_fs_num_sessions = 1;
    
    #define INNER_HEAP_SIZE 0x80000
    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char   nx_inner_heap[INNER_HEAP_SIZE];
    
    void __libnx_initheap(void);
    void __appInit(void);
    void __appExit(void);
}

// Audio globals
AudioOutBuffer audioOutBuffer;
u32 audioBufferSize;
s16* audioBuffer;

void __libnx_initheap(void) {
    void* addr = nx_inner_heap;
    size_t size = nx_inner_heap_size;
    
    extern char* fake_heap_start;
    extern char* fake_heap_end;
    
    fake_heap_start = (char*)addr;
    fake_heap_end   = (char*)addr + size;
}

void __appInit(void) {
    Result rc;
    
    rc = smInitialize();
    if (R_FAILED(rc)) fatalThrow(rc);
    
    // Initialize audio
    rc = audoutInitialize();
    if (R_FAILED(rc)) fatalThrow(rc);
}

void __appExit(void) {
    // Cleanup audio
    if (audioBuffer) {
        free(audioBuffer);
    }
    audoutExit();
    
    smExit();
}

void generateTestTone() {
    const u32 sampleRate = 48000;
    const u32 channelCount = 2;
    const u32 frequency = 440; // A4 note
    const float volume = 0.1f; // Low volume for safety
    const u32 samples = sampleRate; // 1 second
    
    // Allocate buffer
    audioBufferSize = samples * channelCount * sizeof(s16);
    audioBuffer = (s16*)memalign(0x1000, audioBufferSize);
    
    if (!audioBuffer) return;
    
    // Generate sine wave
    for (u32 i = 0; i < samples; i++) {
        float t = (float)i / sampleRate;
        s16 sample = (s16)(32767.0f * volume * sinf(2.0f * M_PI * frequency * t));
        
        // Stereo
        audioBuffer[i * 2] = sample;
        audioBuffer[i * 2 + 1] = sample;
    }
    
    // Setup audio buffer
    audioOutBuffer.next = NULL;
    audioOutBuffer.buffer = audioBuffer;
    audioOutBuffer.buffer_size = audioBufferSize;
    audioOutBuffer.data_size = audioBufferSize;
    audioOutBuffer.data_offset = 0;
}

int main(int argc, char* argv[]) {
    // XMusic v0.1.0-alpha with audio test
    
    // Start audio
    audoutStartAudioOut();
    
    // Generate test tone
    generateTestTone();
    
    // Play test tone once at startup
    if (audioBuffer) {
        audoutAppendAudioOutBuffer(&audioOutBuffer);
        
        // Wait for playback to complete
        AudioOutBuffer* released;
        u32 releasedCount;
        audoutWaitPlayFinish(&released, &releasedCount, UINT64_MAX);
    }
    
    // Service loop
    while (true) {
        svcSleepThread(1000000000LL); // Sleep 1 second
    }
    
    return 0;
}
CPP_END

echo "✅ Audio test code added!"
echo ""
echo "Rebuilding with audio..."
./simple_build.sh

echo ""
echo "🔊 The sysmodule will now play a 440Hz tone (A4 note) for 1 second at startup!"
echo "Volume is set to 10% for safety."