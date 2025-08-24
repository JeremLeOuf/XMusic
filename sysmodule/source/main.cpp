#include <switch.h>
#include <cstring>
#include <cstdio>
#include <memory>
#include "audio_manager.h"
#include "ipc_service.h"

extern "C" {
    u32 __nx_applet_type = AppletType_None;
    u32 __nx_fs_num_sessions = 1;
    
    #define INNER_HEAP_SIZE 0x100000  // 1MB heap
    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char   nx_inner_heap[INNER_HEAP_SIZE];
    
    void __libnx_initheap(void);
    void __appInit(void);
    void __appExit(void);
}

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

int main(int argc, char* argv[]) {
    // XMusic v0.1.0-alpha
    
    // Create audio manager
    auto audioManager = std::make_unique<AudioManager>();
    
    // Load test melody
    audioManager->loadMelody();
    
    // Play startup sound
    audioManager->play();
    svcSleepThread(2000000000LL); // Play for 2 seconds
    audioManager->pause();
    
    // Load continuous test tone for background
    audioManager->loadTestTone(440.0f, 10.0f); // 10 second loop
    
    // Create IPC service
    auto ipcService = std::make_unique<IPCService>(audioManager.get());
    
    Result rc = ipcService->initialize();
    if (R_FAILED(rc)) {
        // Service registration failed - probably already running
        return 1;
    }
    
    // Main service loop
    while (true) {
        svcSleepThread(1000000000LL); // Sleep 1 second
        
        // Could add status checks or maintenance here
    }
    
    // Cleanup (won't reach here normally)
    ipcService->finalize();
    
    return 0;
}