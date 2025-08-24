#include <switch.h>
#include <cstring>
#include <cstdio>

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
}

void __appExit(void) {
    smExit();
}

int main(int argc, char* argv[]) {
    // Simple service that just runs
    // XMusic v0.1.0-alpha
    
    // Service will run as a background process
    while (true) {
        svcSleepThread(1000000000LL); // Sleep 1 second
    }
    
    return 0;
}
