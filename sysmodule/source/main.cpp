#include <switch.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include "audio_manager.h"
#include "xmusic_service.h"
#include "../../common/xmusic_ipc.h"

extern "C" {
    u32 __nx_applet_type = AppletType_None;
    u32 __nx_fs_num_sessions = 1;
    
    #define INNER_HEAP_SIZE 0x200000  // 2MB heap
    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char   nx_inner_heap[INNER_HEAP_SIZE];
    
    void __libnx_initheap(void);
    void __appInit(void);
    void __appExit(void);
}

// Global instances
std::shared_ptr<AudioManager> audioManager;
std::unique_ptr<XMusicService> xmusicService;

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
    
    rc = fsInitialize();
    if (R_FAILED(rc)) fatalThrow(rc);
    
    rc = fsdevMountSdmc();
    if (R_FAILED(rc)) fatalThrow(rc);
    
    rc = audoutInitialize();
    if (R_FAILED(rc)) fatalThrow(rc);
}

void __appExit(void) {
    audoutExit();
    fsdevUnmountAll();
    fsExit();
    smExit();
}

void handleServiceRequest(Handle session) {
    // This function is no longer needed - handled by XMusicService
}

void serviceThread(void* arg) {
    // This function is no longer needed - handled by XMusicService
}

int main(int argc, char* argv[]) {
    // Initialize audio manager
    audioManager = std::make_shared<AudioManager>();
    
    // Play startup sound
    audioManager->loadMelody();
    audioManager->play();
    svcSleepThread(2000000000LL); // 2 seconds
    audioManager->pause();
    
    // Load test tone for background
    audioManager->loadTestTone(440.0f, 10.0f); // 10 second loop
    
    // Create and initialize service
    xmusicService = std::make_unique<XMusicService>();
    Result rc = xmusicService->initialize(audioManager);
    
    if (R_FAILED(rc)) {
        // Service registration failed, continue as audio-only
        while (true) {
            svcSleepThread(1000000000LL); // Sleep 1 second
        }
        return 0;
    }
    
    // Start the service
    rc = xmusicService->start();
    if (R_FAILED(rc)) {
        // Service start failed, continue as audio-only
        while (true) {
            svcSleepThread(1000000000LL); // Sleep 1 second
        }
        return 0;
    }
    
    // Main service loop
    while (true) {
        svcSleepThread(1000000000LL); // Sleep 1 second
        
        // Service status check
        if (!xmusicService->isRunning()) {
            break;
        }
    }
    
    // Cleanup
    xmusicService->stop();
    audioManager.reset();
    
    return 0;
}
