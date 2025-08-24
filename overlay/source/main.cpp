#define TESLA_INIT_IMPL
#include <tesla.hpp>
#include <switch.h>
#include "../../common/xmusic_ipc.h"

class XMusicGui : public tsl::Gui {
private:
    Handle serviceHandle = 0;
    XMusicStatus status = {};
    bool serviceConnected = false;
    
    // UI Elements
    tsl::elm::ListItem* playPauseItem = nullptr;
    tsl::elm::TrackBar* volumeBar = nullptr;
    tsl::elm::ListItem* statusItem = nullptr;
    
public:
    XMusicGui() {
        // Try to connect to XMusic service
        SmServiceName serviceName = smEncodeName(XMUSIC_SERVICE_NAME);
        Result rc = smGetServiceOriginal(&serviceHandle, serviceName);
        
        if (R_SUCCEEDED(rc)) {
            serviceConnected = true;
            updateStatus();
        }
    }
    
    ~XMusicGui() {
        if (serviceHandle) {
            svcCloseHandle(serviceHandle);
        }
    }
    
    virtual tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("XMusic", XMUSIC_VERSION);
        auto list = new tsl::elm::List();
        
        // Header
        list->addItem(new tsl::elm::CategoryHeader("Now Playing"));
        
        // Status
        statusItem = new tsl::elm::ListItem(
            serviceConnected ? 
            (status.title[0] ? status.title : "Ready") : 
            "Service Not Running"
        );
        statusItem->setValue(status.artist[0] ? status.artist : "");
        list->addItem(statusItem);
        
        // Controls header
        list->addItem(new tsl::elm::CategoryHeader("Controls"));
        
        // Play/Pause button
        playPauseItem = new tsl::elm::ListItem(status.playing ? "⏸ Pause" : "▶ Play");
        playPauseItem->setClickListener([this](u64 keys) {
            if (keys & HidNpadButton_A && serviceConnected) {
                if (status.playing) {
                    sendCommand(XMusicCmd_Pause);
                } else {
                    sendCommand(XMusicCmd_Play);
                }
                updateStatus();
                
                // Update button text
                playPauseItem->setText(status.playing ? "⏸ Pause" : "▶ Play");
                return true;
            }
            return false;
        });
        list->addItem(playPauseItem);
        
        // Volume
        list->addItem(new tsl::elm::CategoryHeader("Volume"));
        
        volumeBar = new tsl::elm::TrackBar("\uE13C");
        volumeBar->setProgress(status.volume * 100);
        volumeBar->setClickListener([this](u64 keys) {
            if (keys & HidNpadButton_A && serviceConnected) {
                float newVolume = volumeBar->getProgress() / 100.0f;
                sendVolumeCommand(newVolume);
                return true;
            }
            return false;
        });
        list->addItem(volumeBar);
        
        // Test functions
        list->addItem(new tsl::elm::CategoryHeader("Test Audio"));
        
        auto testToneItem = new tsl::elm::ListItem("Play Test Tone");
        testToneItem->setClickListener([this](u64 keys) {
            if (keys & HidNpadButton_A && serviceConnected) {
                sendCommand(XMusicCmd_Play);
                updateStatus();
                return true;
            }
            return false;
        });
        list->addItem(testToneItem);
        
        // Service status
        list->addItem(new tsl::elm::CategoryHeader("Service Status"));
        
        auto serviceStatusItem = new tsl::elm::ListItem(
            serviceConnected ? "✅ Connected" : "❌ Disconnected"
        );
        serviceStatusItem->setValue(serviceConnected ? "Service running" : "Start sysmodule");
        list->addItem(serviceStatusItem);
        
        // Info
        list->addItem(new tsl::elm::CategoryHeader("Info"));
        
        auto versionItem = new tsl::elm::ListItem("Version");
        versionItem->setValue(XMUSIC_VERSION);
        list->addItem(versionItem);
        
        auto memoryItem = new tsl::elm::ListItem("Memory Usage");
        memoryItem->setValue("~4 MB");
        list->addItem(memoryItem);
        
        frame->setContent(list);
        return frame;
    }
    
    virtual void update() override {
        // Update status periodically
        static int updateCounter = 0;
        if (++updateCounter > 60) { // Every second at 60fps
            updateCounter = 0;
            if (serviceConnected) {
                updateStatus();
                
                // Update UI elements
                if (playPauseItem) {
                    playPauseItem->setText(status.playing ? "⏸ Pause" : "▶ Play");
                }
                if (statusItem && status.title[0]) {
                    statusItem->setText(status.title);
                    statusItem->setValue(status.artist);
                }
                if (volumeBar) {
                    volumeBar->setProgress(status.volume * 100);
                }
            }
        }
    }
    
private:
    void sendCommand(u32 cmd) {
        if (!serviceHandle) return;
        
        IpcCommand c;
        ipcInitialize(&c);
        
        struct {
            u64 magic;
            u64 cmd_id;
        } *raw;
        
        raw = (decltype(raw))ipcPrepareHeader(&c, sizeof(*raw));
        raw->magic = SFCI_MAGIC;
        raw->cmd_id = cmd;
        
        svcSendSyncRequest(serviceHandle);
    }
    
    void sendVolumeCommand(float volume) {
        if (!serviceHandle) return;
        
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddSendStatic(&c, &volume, sizeof(float), 0);
        
        struct {
            u64 magic;
            u64 cmd_id;
        } *raw;
        
        raw = (decltype(raw))ipcPrepareHeader(&c, sizeof(*raw));
        raw->magic = SFCI_MAGIC;
        raw->cmd_id = XMusicCmd_SetVolume;
        
        svcSendSyncRequest(serviceHandle);
    }
    
    void updateStatus() {
        if (!serviceHandle) return;
        
        IpcCommand c;
        ipcInitialize(&c);
        ipcAddRecvStatic(&c, &status, sizeof(XMusicStatus), 0);
        
        struct {
            u64 magic;
            u64 cmd_id;
        } *raw;
        
        raw = (decltype(raw))ipcPrepareHeader(&c, sizeof(*raw));
        raw->magic = SFCI_MAGIC;
        raw->cmd_id = XMusicCmd_GetStatus;
        
        Result rc = svcSendSyncRequest(serviceHandle);
        if (R_SUCCEEDED(rc)) {
            IpcParsedCommand r;
            ipcParse(&r);
        }
    }
};

class XMusicOverlay : public tsl::Overlay {
public:
    virtual void initServices() override {
        smInitialize();
    }
    
    virtual void exitServices() override {
        smExit();
    }
    
    virtual void onShow() override {}
    virtual void onHide() override {}
    
    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return std::make_unique<XMusicGui>();
    }
};

int main(int argc, char **argv) {
    return tsl::loop<XMusicOverlay>(argc, argv);
}