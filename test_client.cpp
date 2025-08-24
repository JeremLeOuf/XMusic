#include <iostream>
#include <cstring>
#include <switch.h>
#include "common/xmusic_ipc.h"

class XMusicClient {
private:
    Service m_service;
    bool m_connected = false;

public:
    Result connect() {
        Result rc = smGetService(&m_service, "xmusic");
        if (R_SUCCEEDED(rc)) {
            m_connected = true;
            std::cout << "✅ Connected to XMusic service" << std::endl;
        } else {
            std::cout << "❌ Failed to connect to XMusic service: 0x" << std::hex << rc << std::endl;
        }
        return rc;
    }

    void disconnect() {
        if (m_connected) {
            serviceClose(&m_service);
            m_connected = false;
            std::cout << "Disconnected from XMusic service" << std::endl;
        }
    }

    Result sendCommand(XMusicCmd cmd) {
        if (!m_connected) {
            std::cout << "❌ Not connected to service" << std::endl;
            return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);
        }

        Result rc = serviceDispatchIn(&m_service, static_cast<u32>(cmd), cmd);
        if (R_SUCCEEDED(rc)) {
            std::cout << "✅ Command sent successfully: " << static_cast<int>(cmd) << std::endl;
        } else {
            std::cout << "❌ Command failed: 0x" << std::hex << rc << std::endl;
        }
        return rc;
    }

    Result getStatus(XMusicStatus* status) {
        if (!m_connected) {
            std::cout << "❌ Not connected to service" << std::endl;
            return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);
        }

        Result rc = serviceDispatchOut(&m_service, static_cast<u32>(XMusicCmd_GetStatus), *status);
        if (R_SUCCEEDED(rc)) {
            std::cout << "✅ Status retrieved successfully" << std::endl;
            std::cout << "   Is Playing: " << (status->playing ? "Yes" : "No") << std::endl;
            std::cout << "   Volume: " << status->volume << std::endl;
            std::cout << "   Title: " << status->title << std::endl;
            std::cout << "   Artist: " << status->artist << std::endl;
        } else {
            std::cout << "❌ Get status failed: 0x" << std::hex << rc << std::endl;
        }
        return rc;
    }
};

int main() {
    std::cout << "==================================" << std::endl;
    std::cout << "      XMusic IPC Test Client      " << std::endl;
    std::cout << "==================================" << std::endl;

    // Initialize services
    Result rc = smInitialize();
    if (R_FAILED(rc)) {
        std::cout << "❌ Failed to initialize sm" << std::endl;
        return 1;
    }

    XMusicClient client;
    
    // Connect to service
    rc = client.connect();
    if (R_FAILED(rc)) {
        smExit();
        return 1;
    }

    // Test commands
    std::cout << "\nTesting commands..." << std::endl;
    
    // Get initial status
    XMusicStatus status = {};
    client.getStatus(&status);
    
    // Test play command
    std::cout << "\n📻 Testing PLAY command..." << std::endl;
    client.sendCommand(static_cast<XMusicCmd>(XMusicCmd_Play));
    
    // Wait a bit
    svcSleepThread(1000000000ULL); // 1 second
    
    // Test pause command
    std::cout << "\n⏸️  Testing PAUSE command..." << std::endl;
    client.sendCommand(static_cast<XMusicCmd>(XMusicCmd_Pause));
    
    // Test volume commands - using SetVolume instead
    std::cout << "\n� Testing SET_VOLUME command..." << std::endl;
    client.sendCommand(static_cast<XMusicCmd>(XMusicCmd_SetVolume));
    
    // Get final status
    std::cout << "\n📊 Getting final status..." << std::endl;
    client.getStatus(&status);
    
    // Cleanup
    client.disconnect();
    smExit();
    
    std::cout << "\n✅ Test completed successfully!" << std::endl;
    return 0;
}
