#include <switch.h>
#include <iostream>
#include <string>
#include <cstring>
#include "../common/xmusic_ipc.h"

// Input handling globals
PadState pad;

class XMusicController {
private:
    Service m_service;
    bool m_connected = false;
    
public:
    XMusicController() {
        // Initialize services
        if (R_SUCCEEDED(smInitialize())) {
            Result rc = smGetService(&m_service, XMUSIC_SERVICE_NAME);
            if (R_SUCCEEDED(rc)) {
                m_connected = true;
                std::cout << "✅ Connected to XMusic service" << std::endl;
            } else {
                std::cout << "❌ Failed to connect to XMusic service" << std::endl;
                std::cout << "   Make sure XMusic sysmodule is running" << std::endl;
            }
        }
    }
    
    ~XMusicController() {
        if (m_connected) {
            serviceClose(&m_service);
        }
        smExit();
    }
    
    bool isConnected() const { return m_connected; }
    
    Result sendCommand(XMusicCmd cmd) {
        if (!m_connected) return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);
        return serviceDispatchIn(&m_service, static_cast<u32>(cmd), cmd);
    }
    
    Result getStatus(XMusicStatus* status) {
        if (!m_connected) return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);
        return serviceDispatchOut(&m_service, static_cast<u32>(XMusicCmd_GetStatus), *status);
    }
};

void printMenu() {
    std::cout << "\n================================" << std::endl;
    std::cout << "      🎵 XMusic Controller     " << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << "A - Play/Pause" << std::endl;
    std::cout << "X - Next Track" << std::endl;
    std::cout << "Y - Previous Track" << std::endl;
    std::cout << "L/R - Volume Down/Up" << std::endl;
    std::cout << "ZL - Get Status" << std::endl;
    std::cout << "+ - Exit" << std::endl;
    std::cout << "================================" << std::endl;
}

void printStatus(const XMusicStatus& status) {
    std::cout << "\n📊 XMusic Status:" << std::endl;
    std::cout << "   Playing: " << (status.playing ? "Yes" : "No") << std::endl;
    std::cout << "   Volume: " << (int)(status.volume * 100) << "%" << std::endl;
    
    if (strlen(status.title) > 0) {
        std::cout << "   Title: " << status.title << std::endl;
    }
    
    if (strlen(status.artist) > 0) {
        std::cout << "   Artist: " << status.artist << std::endl;
    }
    
    if (status.duration > 0) {
        std::cout << "   Position: " << status.position << "/" << status.duration << "s" << std::endl;
    }
}

int main() {
    // Initialize console
    consoleInit(NULL);
    
    // Initialize input
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&pad);
    
    // Initialize controller
    XMusicController controller;
    
    if (!controller.isConnected()) {
        std::cout << "\n❌ Cannot continue without XMusic service" << std::endl;
        std::cout << "Press any button to exit..." << std::endl;
        
        while (appletMainLoop()) {
            padUpdate(&pad);
            u64 kDown = padGetButtonsDown(&pad);
            if (kDown) break;
        }
        
        consoleExit(NULL);
        return 1;
    }
    
    printMenu();
    
    XMusicStatus currentStatus = {};
    bool statusVisible = false;
    
    // Main loop
    while (appletMainLoop()) {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        
        if (kDown & HidNpadButton_Plus) {
            break; // Exit
        }
        
        Result rc = 0;
        bool commandSent = false;
        
        if (kDown & HidNpadButton_A) {
            // Toggle play/pause - first get status to know current state
            controller.getStatus(&currentStatus);
            rc = controller.sendCommand(currentStatus.playing ? XMusicCmd_Pause : XMusicCmd_Play);
            std::cout << (currentStatus.playing ? "⏸️ Pausing..." : "▶️ Playing...") << std::endl;
            commandSent = true;
        }
        
        if (kDown & HidNpadButton_X) {
            rc = controller.sendCommand(XMusicCmd_Next);
            std::cout << "⏭️ Next track..." << std::endl;
            commandSent = true;
        }
        
        if (kDown & HidNpadButton_Y) {
            rc = controller.sendCommand(XMusicCmd_Previous);
            std::cout << "⏮️ Previous track..." << std::endl;
            commandSent = true;
        }
        
        if (kDown & HidNpadButton_L) {
            // Volume down - using SetVolume (we'll improve this later)
            rc = controller.sendCommand(XMusicCmd_SetVolume);
            std::cout << "🔉 Volume down..." << std::endl;
            commandSent = true;
        }
        
        if (kDown & HidNpadButton_R) {
            // Volume up - using SetVolume (we'll improve this later)  
            rc = controller.sendCommand(XMusicCmd_SetVolume);
            std::cout << "🔊 Volume up..." << std::endl;
            commandSent = true;
        }
        
        if (kDown & HidNpadButton_ZL) {
            rc = controller.getStatus(&currentStatus);
            if (R_SUCCEEDED(rc)) {
                printStatus(currentStatus);
                statusVisible = true;
            } else {
                std::cout << "❌ Failed to get status" << std::endl;
            }
        }
        
        if (commandSent) {
            if (R_SUCCEEDED(rc)) {
                std::cout << "✅ Command sent successfully" << std::endl;
            } else {
                std::cout << "❌ Command failed: 0x" << std::hex << rc << std::endl;
            }
        }
        
        // Update status periodically
        if (statusVisible) {
            controller.getStatus(&currentStatus);
        }
        
        consoleUpdate(NULL);
    }
    
    std::cout << "\n👋 XMusic Controller exiting..." << std::endl;
    consoleUpdate(NULL);
    consoleExit(NULL);
    
    return 0;
}
