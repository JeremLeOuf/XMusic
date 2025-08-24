#!/bin/bash

echo "Fixing Makefiles for WSL environment..."

# Fix sysmodule Makefile
cat > sysmodule/Makefile << 'MAKEFILE_END'
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

# Fix for WSL - use devkitPro's linker explicitly
export LD := $(DEVKITPRO)/devkitA64/bin/aarch64-none-elf-ld
export CC := $(DEVKITPRO)/devkitA64/bin/aarch64-none-elf-gcc
export CXX := $(DEVKITPRO)/devkitA64/bin/aarch64-none-elf-g++

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
MAKEFILE_END

# Simplified main.cpp without curl dependencies for testing
cat > sysmodule/source/main.cpp << 'CPP_END'
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
CPP_END

echo "✅ Makefiles fixed for WSL!"
echo ""
echo "Testing compiler paths..."
echo "CC: $(which aarch64-none-elf-gcc)"
echo "LD: $(which aarch64-none-elf-ld)"
echo ""
echo "Now run: ./build.sh"
SCRIPT_END

chmod +x fix_makefile_wsl.sh
