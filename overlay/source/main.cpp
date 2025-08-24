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
