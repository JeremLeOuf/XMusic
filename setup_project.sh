#!/bin/bash

echo "========================================"
echo "      XMusic Project Setup Script      "
echo "========================================"

# Create directory structure
echo "Creating project structure..."
mkdir -p sysmodule/source
mkdir -p sysmodule/build
mkdir -p overlay/source
mkdir -p overlay/build
mkdir -p common
mkdir -p atmosphere/contents/58000000000000A1
mkdir -p libs
mkdir -p dist
mkdir -p .github/workflows
mkdir -p docs

# Create common header file
echo "Creating common/xmusic_ipc.h..."
cat > common/xmusic_ipc.h << 'EOF'
#pragma once
#include <switch.h>
#include <cstring>

#define XMUSIC_SERVICE_NAME "xmusic"
#define XMUSIC_VERSION "0.1.0-alpha"

enum XMusicCmd : u32 {
    XMusicCmd_Play = 0,
    XMusicCmd_Pause = 1,
    XMusicCmd_Next = 2,
    XMusicCmd_Previous = 3,
    XMusicCmd_GetStatus = 4,
    XMusicCmd_Search = 5,
    XMusicCmd_SetVolume = 6,
    XMusicCmd_PlayUrl = 7
};

struct XMusicStatus {
    bool playing;
    char title[128];
    char artist[64];
    u32 position;
    u32 duration;
    float volume;
};
EOF

# Create sysmodule Makefile
echo "Creating sysmodule/Makefile..."
cat > sysmodule/Makefile << 'EOF'
include $(DEVKITPRO)/libnx/switch_rules

TARGET := xmusic
BUILD := build
SOURCES := source
INCLUDES := ../common

ARCH := -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS := -g -Wall -O2 -ffunction-sections $(ARCH) $(DEFINES)
CFLAGS += $(INCLUDE) -D__SWITCH__

CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++17

ASFLAGS := -g $(ARCH)
LDFLAGS = -specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS := -lnx -lcurl -lz -lmbedtls -lm

LIBDIRS := $(PORTLIBS) $(LIBNX)

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT := $(CURDIR)/$(TARGET)
export TOPDIR := $(CURDIR)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)

CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

export OFILES_SRC := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES := $(OFILES_SRC)

export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
                   $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                   -I$(CURDIR)/$(BUILD)

export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean all

all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).nso $(TARGET).elf

else

.PHONY: all

DEPENDS := $(OFILES:.o=.d)

all: $(OUTPUT).nso

$(OUTPUT).nso: $(OUTPUT).elf
	@elf2nso $(OUTPUT).elf $(OUTPUT).nso
	@echo built ... $(notdir $@)

$(OUTPUT).elf: $(OFILES)

-include $(DEPENDS)

endif
EOF

# Create main.cpp for sysmodule
echo "Creating sysmodule/source/main.cpp..."
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
    // Simple test - service will register and run
    Service srv;
    Result rc = smRegisterService(&srv, XMUSIC_SERVICE_NAME, false, 1);
    
    if (R_SUCCEEDED(rc)) {
        // Service is running
        while (true) {
            svcSleepThread(1000000000LL); // Sleep 1 second
        }
        
        serviceClose(&srv);
        smUnregisterService(XMUSIC_SERVICE_NAME);
    }
    
    return 0;
}
EOF

# Create overlay Makefile
echo "Creating overlay/Makefile..."
cat > overlay/Makefile << 'EOF'
include $(DEVKITPRO)/libnx/switch_rules

TARGET := xmusic-overlay
BUILD := build
SOURCES := source
INCLUDES := ../common

ARCH := -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS := -g -Wall -O2 -ffunction-sections $(ARCH) $(DEFINES)
CFLAGS += $(INCLUDE) -D__SWITCH__

CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++17

ASFLAGS := -g $(ARCH)
LDFLAGS = -specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS := -lnx

LIBDIRS := $(PORTLIBS) $(LIBNX)

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT := $(CURDIR)/$(TARGET)
export TOPDIR := $(CURDIR)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)

CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

export OFILES_SRC := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES := $(OFILES_SRC)

export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
                   $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                   -I$(CURDIR)/$(BUILD)

export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean all

all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).ovl $(TARGET).elf

else

.PHONY: all

DEPENDS := $(OFILES:.o=.d)

all: $(OUTPUT).ovl

$(OUTPUT).ovl: $(OUTPUT).elf
	@elf2ovl $(OUTPUT).elf $(OUTPUT).ovl
	@echo built ... $(notdir $@)

$(OUTPUT).elf: $(OFILES)

-include $(DEPENDS)

endif
EOF

# Create simple overlay main
echo "Creating overlay/source/main.cpp..."
cat > overlay/source/main.cpp << 'EOF'
#include <switch.h>
#include <cstring>
#include "../../common/xmusic_ipc.h"

// Simple test overlay
int main(int argc, char* argv[]) {
    // For now, just a placeholder
    // Tesla integration will be added later
    return 0;
}
EOF

# Create toolbox.json
echo "Creating atmosphere/contents/58000000000000A1/toolbox.json..."
cat > atmosphere/contents/58000000000000A1/toolbox.json << 'EOF'
{
    "name": "XMusic",
    "tid": "58000000000000A1",
    "requires_reboot": false,
    "version": "0.1.0-alpha"
}
EOF

# Create root Makefile
echo "Creating root Makefile..."
cat > Makefile << 'EOF'
.PHONY: all clean sysmodule overlay

all: sysmodule overlay

sysmodule:
	@$(MAKE) -C sysmodule

overlay:
	@$(MAKE) -C overlay

clean:
	@$(MAKE) -C sysmodule clean
	@$(MAKE) -C overlay clean
	@rm -rf dist
EOF

echo ""
echo "========================================"
echo "✅ Project structure created!"
echo "========================================"
echo ""
echo "Project tree:"
find . -type f -name "*.cpp" -o -name "*.h" -o -name "Makefile" -o -name "*.json" | head -20
echo ""
echo "Next: Run ./build.sh to compile XMusic"
EOF

chmod +x setup_project.sh
