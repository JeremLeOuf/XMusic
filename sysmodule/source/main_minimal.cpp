#include <switch.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include "audio_manager_fixed.h"
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
std::unique_ptr<AudioManager> audioManager;
Handle serviceHandle;
SmServiceName serviceName;
XMusicStatus currentStatus = {};

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
    // Simplified service handler - just close the session for now
    // We'll implement proper IPC later when the libnx API is figured out
    svcCloseHandle(session);
}

void serviceThread(void* arg) {
    while (true) {
        Handle session;
        Result rc = svcAcceptSession(&session, serviceHandle);
        
        if (R_SUCCEEDED(rc)) {
            handleServiceRequest(session);
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialize audio manager
    audioManager = std::make_unique<AudioManager>();
    
    // Play startup sound
    audioManager->loadMelody();
    audioManager->play();
    svcSleepThread(2000000000LL); // 2 seconds
    audioManager->pause();
    
    // Load test tone for background
    audioManager->loadTestTone(440.0f, 10.0f); // 10 second loop
    
    // Initialize status
    strcpy(currentStatus.title, "XMusic Ready");
    strcpy(currentStatus.artist, "System");
    currentStatus.volume = 0.3f;
    currentStatus.playing = false;
    
    // Register service
    serviceName = smEncodeName(XMUSIC_SERVICE_NAME);
    Result rc = smRegisterService(&serviceHandle, serviceName, false, 2);
    
    if (R_FAILED(rc)) {
        // Service already registered, just run as audio-only
        // Main service loop
        while (true) {
            svcSleepThread(1000000000LL); // Sleep 1 second
        }
        return 0;
    }
    
    // Create service thread
    Thread thread;
    rc = threadCreate(&thread, serviceThread, nullptr, nullptr, 0x4000, 0x2B, -2);
    if (R_SUCCEEDED(rc)) {
        rc = threadStart(&thread);
    }
    
    // Main service loop
    while (true) {
        svcSleepThread(1000000000LL); // Sleep 1 second
        
        // Could add status checks or maintenance here
    }
    
    // Cleanup (won't reach here normally)
    audioManager.reset();
    
    return 0;
}
