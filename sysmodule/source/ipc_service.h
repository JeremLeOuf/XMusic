#pragma once
#include <switch.h>
#include <cstring>
#include "../../common/xmusic_ipc.h"
#include "audio_manager.h"

class IPCService {
private:
    Handle serviceHandle;
    Handle sessionHandle;
    SmServiceName serviceName;
    AudioManager* audioManager;
    XMusicStatus currentStatus;
    
    std::thread serviceThread;
    std::atomic<bool> shouldExit{false};
    
    void serviceThreadFunc() {
        s32 idx;
        Handle handles[2];
        handles[0] = serviceHandle;
        
        while (!shouldExit) {
            Result rc = svcWaitSynchronization(&idx, handles, 1, 100000000);
            
            if (R_SUCCEEDED(rc) && idx == 0) {
                // Accept new session
                Handle session;
                rc = svcAcceptSession(&session, serviceHandle);
                if (R_SUCCEEDED(rc)) {
                    handleSession(session);
                    svcCloseHandle(session);
                }
            }
        }
    }
    
    void handleSession(Handle session) {
        IpcParsedCommand r;
        IpcCommand c;
        
        u8* ptr = (u8*)armGetTls();
        
        while (!shouldExit) {
            Result rc = svcReplyAndReceive(&idx, &session, 1, 0, 100000000);
            
            if (R_FAILED(rc)) break;
            
            rc = ipcParse(&r);
            if (R_FAILED(rc)) continue;
            
            u32 cmd = r.CommandId;
            
            switch (cmd) {
                case XMusicCmd_Play:
                    audioManager->play();
                    currentStatus.playing = true;
                    ipcInitialize(&c);
                    rc = 0;
                    break;
                    
                case XMusicCmd_Pause:
                    audioManager->pause();
                    currentStatus.playing = false;
                    ipcInitialize(&c);
                    rc = 0;
                    break;
                    
                case XMusicCmd_SetVolume:
                    if (r.HasInput) {
                        float vol = *(float*)r.Raw;
                        audioManager->setVolume(vol);
                        currentStatus.volume = vol;
                    }
                    ipcInitialize(&c);
                    rc = 0;
                    break;
                    
                case XMusicCmd_GetStatus:
                    updateStatus();
                    ipcInitialize(&c);
                    ipcAddSendStatic(&c, &currentStatus, sizeof(XMusicStatus), 0);
                    rc = 0;
                    break;
                    
                default:
                    rc = MAKERESULT(Module_Libnx, LibnxError_NotFound);
                    break;
            }
            
            if (rc == 0) {
                ipcPrepareHeader(&c, rc);
                svcSendSyncRequest(session);
            }
        }
    }
    
    void updateStatus() {
        currentStatus.playing = audioManager->getIsPlaying();
        currentStatus.volume = audioManager->getVolume();
        currentStatus.position = audioManager->getProgress() * 100;
        
        if (!currentStatus.title[0]) {
            strcpy(currentStatus.title, "XMusic Test Audio");
            strcpy(currentStatus.artist, "System");
        }
    }
    
    s32 idx;
    
public:
    IPCService(AudioManager* am) : audioManager(am) {
        memset(&currentStatus, 0, sizeof(currentStatus));
        currentStatus.volume = 0.3f;
    }
    
    Result initialize() {
        serviceName = smEncodeName(XMUSIC_SERVICE_NAME);
        
        Result rc = smRegisterService(&serviceHandle, serviceName, false, 2);
        if (R_FAILED(rc)) {
            return rc;
        }
        
        serviceThread = std::thread(&IPCService::serviceThreadFunc, this);
        return 0;
    }
    
    void finalize() {
        shouldExit = true;
        if (serviceThread.joinable()) {
            serviceThread.join();
        }
        
        if (serviceHandle) {
            smUnregisterService(serviceName);
            svcCloseHandle(serviceHandle);
        }
    }
};