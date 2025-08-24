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
    
    // Initialize network for future streaming
    rc = socketInitializeDefault();
    // Don't fail if network not available
    
    rc = nifmInitialize(NifmServiceType_User);
    // Don't fail if network not available
}

void __appExit(void) {
    nifmExit();
    socketExit();
    audoutExit();
    fsdevUnmountAll();
    fsExit();
    smExit();
}

void handleServiceRequest(Handle session) {
    IpcParsedCommand r;
    IpcCommand c;
    
    Result rc = ipcParse(&r);
    if (R_FAILED(rc)) return;
    
    u32 cmd = r.CommandId;
    
    switch (cmd) {
        case XMusicCmd_Play:
            if (audioManager) {
                audioManager->play();
                currentStatus.playing = true;
            }
            ipcInitialize(&c);
            break;
            
        case XMusicCmd_Pause:
            if (audioManager) {
                audioManager->pause();
                currentStatus.playing = false;
            }
            ipcInitialize(&c);
            break;
            
        case XMusicCmd_SetVolume:
            // Volume would be in the request data
            if (r.HasInput) {
                float* vol = (float*)r.Raw;
                if (vol) {
                    audioManager->setVolume(*vol);
                    currentStatus.volume = *vol;
                }
            }
            ipcInitialize(&c);
            break;
            
        case XMusicCmd_GetStatus:
            // Update status
            if (audioManager) {
                currentStatus.playing = audioManager->getIsPlaying();
                currentStatus.volume = audioManager->getVolume();
                currentStatus.position = audioManager->getProgress() * 100;
            }
            
            // Send status back
            ipcInitialize(&c);
            ipcAddSendStatic(&c, &currentStatus, sizeof(XMusicStatus), 0);
            break;
            
        case XMusicCmd_Next:
        case XMusicCmd_Previous:
            // For now, just replay the melody
            if (audioManager) {
                audioManager->loadMelody();
                audioManager->play();
                currentStatus.playing = true;
            }
            ipcInitialize(&c);
            break;
            
        default:
            ipcInitialize(&c);
            break;
    }
    
    ipcPrepareHeader(&c, 0);
}

void serviceThread(void* arg) {
    while (true) {
        Handle session;
        Result rc = svcAcceptSession(&session, serviceHandle);
        
        if (R_SUCCEEDED(rc)) {
            handleServiceRequest(session);
            svcCloseHandle(session);
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
        // Service already registered
        return 1;
    }
    
    // Create service thread
    Thread thread;
    rc = threadCreate(&thread, serviceThread, nullptr, nullptr, 0x4000, 0x2B, -2);
    if (R_SUCCEEDED(rc)) {
        rc = threadStart(&thread);
    }
    
    // Main loop
    while (true) {
        svcSleepThread(1000000000LL); // 1 second
        
        // Update status
        if (audioManager) {
            currentStatus.position = audioManager->getProgress() * 100;
        }
    }
    
    // Cleanup (won't reach here)
    smUnregisterService(serviceName);
    svcCloseHandle(serviceHandle);
    
    return 0;
}