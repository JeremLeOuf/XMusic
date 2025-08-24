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
