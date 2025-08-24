#!/bin/bash

echo "Fixing XMusic source files..."

# Fix main.cpp
cat > sysmodule/source/main.cpp << 'EOF'
#include <switch.h>
#include <cstring>
#include <cstdio>
#include "../../common/xmusic_ipc.h"

extern "C" {
    u32 __nx_applet_type = AppletType_None;
    u32 __nx_fs_num_sessions = 1;
    
    #define INNER_HEAP_SIZE 0x80000
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
}

void __appExit(void) {
    fsdevUnmountAll();
    fsExit();
    smExit();
}

int main(int argc, char* argv[]) {
    // Create service name
    SmServiceName service_name = smEncodeName(XMUSIC_SERVICE_NAME);
    Handle serviceHandle;
    
    // Register the service
    Result rc = smRegisterService(&serviceHandle, service_name, false, 1);
    
    if (R_SUCCEEDED(rc)) {
        // Service is running - simple loop for now
        while (true) {
            svcSleepThread(1000000000LL); // Sleep 1 second
        }
        
        // Cleanup (won't reach here in this simple version)
        svcCloseHandle(serviceHandle);
        smUnregisterService(service_name);
    }
    
    return 0;
}
EOF

# Fix overlay main.cpp to be even simpler
cat > overlay/source/main.cpp << 'EOF'
#include <switch.h>

// Simplified overlay - just a placeholder for now
extern "C" {
    u32 __nx_applet_type = AppletType_None;
    
    void __libnx_initheap(void) {}
    void __appInit(void) {}
    void __appExit(void) {}
}

int main(int argc, char* argv[]) {
    return 0;
}
EOF

echo "✅ Files fixed!"
echo "Now run: ./build.sh"
EOF

chmod +x fix_files.sh
