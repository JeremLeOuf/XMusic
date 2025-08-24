#include "xmusic_service.h"

// Static instance
XMusicService* XMusicService::s_instance = nullptr;

XMusicService::XMusicService() 
    : m_initialized(false), m_running(false), m_handle(INVALID_HANDLE) {
    memset(&m_currentStatus, 0, sizeof(m_currentStatus));
    strcpy(m_currentStatus.title, "XMusic Ready");
    strcpy(m_currentStatus.artist, "System");
    m_currentStatus.volume = 0.3f;
    m_currentStatus.playing = false;
}

XMusicService::~XMusicService() {
    stop();
}

Result XMusicService::initialize(std::shared_ptr<AudioManager> audioManager) {
    if (m_initialized) {
        return 0;
    }
    
    m_audioManager = audioManager;
    
    // Register service using modern API
    SmServiceName serviceName = smEncodeName(XMUSIC_SERVICE_NAME);
    Result rc = smRegisterService(&m_handle, serviceName, false, 2);
    
    if (R_FAILED(rc)) {
        return rc;
    }
    
    m_initialized = true;
    return 0;
}

Result XMusicService::start() {
    if (!m_initialized || m_running) {
        return MAKERESULT(Module_Libnx, LibnxError_BadInput);
    }
    
    m_running = true;
    
    // Start service thread
    m_serviceThread = std::thread(&XMusicService::serviceThreadFunc, this);
    
    return 0;
}

void XMusicService::stop() {
    if (m_running) {
        m_running = false;
        
        if (m_serviceThread.joinable()) {
            m_serviceThread.join();
        }
    }
    
    if (m_initialized && m_handle != INVALID_HANDLE) {
        SmServiceName serviceName = smEncodeName(XMUSIC_SERVICE_NAME);
        smUnregisterService(serviceName);
        svcCloseHandle(m_handle);
        m_handle = INVALID_HANDLE;
        m_initialized = false;
    }
}

void XMusicService::serviceThreadFunc() {
    while (m_running) {
        Handle session;
        Result rc = svcAcceptSession(&session, m_handle);
        
        if (R_SUCCEEDED(rc)) {
            // Handle the request in a separate scope
            handleRequest(session);
            svcCloseHandle(session);
        } else {
            // Sleep briefly on failure to avoid busy loop
            svcSleepThread(1000000); // 1ms
        }
    }
}

Result XMusicService::handleRequest(Handle session) {
    if (!m_audioManager) {
        return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);
    }
    
    // For now, implement a simple protocol
    // We'll use svcReplyAndReceive for basic communication
    
    s32 idx;
    Result rc = svcReplyAndReceive(&idx, &session, 1, 0, 1000000000LL); // 1 second timeout
    
    if (R_FAILED(rc)) {
        return rc;
    }
    
    // Read the command from TLS (Thread Local Storage)
    // This is a simplified approach - we'll read the command ID directly
    u8* tls = (u8*)armGetTls();
    u32* cmdPtr = (u32*)(tls + 8); // Command typically at offset 8
    u32 cmd = *cmdPtr;
    
    // Process the command
    switch (cmd) {
        case XMusicCmd_Play:
            return cmdPlay(session);
            
        case XMusicCmd_Pause:
            return cmdPause(session);
            
        case XMusicCmd_GetStatus:
            return cmdGetStatus(session);
            
        case XMusicCmd_SetVolume:
            // For now, use a default volume
            return cmdSetVolume(session, 0.5f);
            
        case XMusicCmd_Next:
        case XMusicCmd_Previous:
            return cmdLoadMelody(session);
            
        default:
            // Unknown command, just return success
            return 0;
    }
}

Result XMusicService::cmdPlay(Handle session) {
    if (m_audioManager) {
        m_audioManager->play();
        m_currentStatus.playing = true;
        updateStatus();
    }
    return 0;
}

Result XMusicService::cmdPause(Handle session) {
    if (m_audioManager) {
        m_audioManager->pause();
        m_currentStatus.playing = false;
        updateStatus();
    }
    return 0;
}

Result XMusicService::cmdSetVolume(Handle session, float volume) {
    if (m_audioManager) {
        m_audioManager->setVolume(volume);
        m_currentStatus.volume = volume;
        updateStatus();
    }
    return 0;
}

Result XMusicService::cmdGetStatus(Handle session) {
    updateStatus();
    
    // For now, we can't easily send back the status
    // This would require proper IPC response handling
    // We'll implement this in the next iteration
    
    return 0;
}

Result XMusicService::cmdLoadMelody(Handle session) {
    if (m_audioManager) {
        m_audioManager->loadMelody();
        m_audioManager->play();
        m_currentStatus.playing = true;
        updateStatus();
    }
    return 0;
}

void XMusicService::updateStatus() {
    if (m_audioManager) {
        m_currentStatus.playing = m_audioManager->getIsPlaying();
        m_currentStatus.volume = m_audioManager->getVolume();
        m_currentStatus.position = (u32)(m_audioManager->getProgress() * 100);
    }
}

XMusicService* XMusicService::getInstance() {
    if (!s_instance) {
        s_instance = new XMusicService();
    }
    return s_instance;
}

void XMusicService::destroyInstance() {
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}
