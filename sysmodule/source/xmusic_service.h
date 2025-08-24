#pragma once
#include <switch.h>
#include <cstring>
#include <atomic>
#include <thread>
#include <memory>
#include "../../common/xmusic_ipc.h"
#include "audio_manager.h"

/**
 * XMusic IPC Service Handler
 * 
 * Modern libnx service implementation using the Service framework
 * instead of the old IPC API that no longer exists.
 */
class XMusicService {
private:
    static XMusicService* s_instance;
    
    Service m_service;
    bool m_initialized;
    std::atomic<bool> m_running;
    Handle m_handle;
    std::thread m_serviceThread;
    
    // Audio manager reference
    std::shared_ptr<AudioManager> m_audioManager;
    
    // Current status
    XMusicStatus m_currentStatus;
    
    /**
     * Service thread function - handles incoming IPC requests
     */
    void serviceThreadFunc();
    
    /**
     * Handle a single IPC request
     */
    Result handleRequest(Handle session);
    
    /**
     * Process specific commands
     */
    Result cmdPlay(Handle session);
    Result cmdPause(Handle session);
    Result cmdSetVolume(Handle session, float volume);
    Result cmdGetStatus(Handle session);
    Result cmdLoadMelody(Handle session);
    
    /**
     * Update internal status from audio manager
     */
    void updateStatus();

public:
    XMusicService();
    ~XMusicService();
    
    /**
     * Initialize the service
     */
    Result initialize(std::shared_ptr<AudioManager> audioManager);
    
    /**
     * Start the service thread
     */
    Result start();
    
    /**
     * Stop the service
     */
    void stop();
    
    /**
     * Check if service is running
     */
    bool isRunning() const { return m_running; }
    
    /**
     * Get singleton instance
     */
    static XMusicService* getInstance();
    
    /**
     * Destroy singleton instance
     */
    static void destroyInstance();
};
