#pragma once
#include <switch.h>
#include <string>
#include <thread>
#include <atomic>
#include <curl/curl.h>
#include <mpg123.h>

class AudioPlayer {
private:
    static constexpr int SAMPLE_RATE = 48000;
    static constexpr int CHANNELS = 2;
    static constexpr int SAMPLES_PER_BUF = 4096;
    
    AudioOutBuffer audiobuf[2];
    u8* buf_data[2];
    int cur_buf = 0;
    
    AudioOutBuffer* released_buf = nullptr;
    u32 released_count;
    
    mpg123_handle* mpg = nullptr;
    CURL* curl = nullptr;
    
    std::thread playThread;
    std::atomic<bool> playing{false};
    std::atomic<bool> shouldStop{false};
    std::atomic<float> volume{0.5f};
    
    std::string currentUrl;
    
    // Curl callback for streaming
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        AudioPlayer* player = static_cast<AudioPlayer*>(userp);
        size_t realsize = size * nmemb;
        
        if (player->mpg) {
            mpg123_feed(player->mpg, (const unsigned char*)contents, realsize);
        }
        
        return realsize;
    }
    
    void playbackThread() {
        while (!shouldStop) {
            if (playing && mpg) {
                size_t done;
                int err = mpg123_read(mpg, buf_data[cur_buf], SAMPLES_PER_BUF * CHANNELS * sizeof(s16), &done);
                
                if (err == MPG123_OK || err == MPG123_NEW_FORMAT) {
                    // Apply volume
                    s16* samples = (s16*)buf_data[cur_buf];
                    for (size_t i = 0; i < done / sizeof(s16); i++) {
                        samples[i] = samples[i] * volume;
                    }
                    
                    // Submit buffer
                    audiobuf[cur_buf].data_size = done;
                    audoutAppendAudioOutBuffer(&audiobuf[cur_buf]);
                    
                    // Wait for playback
                    audoutWaitPlayFinish(&released_buf, &released_count, UINT64_MAX);
                    
                    cur_buf = (cur_buf + 1) % 2;
                } else if (err == MPG123_NEED_MORE) {
                    // Need more data from curl
                    svcSleepThread(10000000); // 10ms
                }
            } else {
                svcSleepThread(100000000); // 100ms
            }
        }
    }
    
public:
    AudioPlayer() {
        // Initialize audio output
        audoutInitialize();
        audoutStartAudioOut();
        
        // Allocate buffers
        for (int i = 0; i < 2; i++) {
            buf_data[i] = (u8*)aligned_alloc(0x1000, SAMPLES_PER_BUF * CHANNELS * sizeof(s16));
            audiobuf[i].next = nullptr;
            audiobuf[i].buffer = buf_data[i];
            audiobuf[i].buffer_size = SAMPLES_PER_BUF * CHANNELS * sizeof(s16);
            audiobuf[i].data_size = 0;
            audiobuf[i].data_offset = 0;
        }
        
        // Initialize mpg123
        mpg123_init();
        mpg = mpg123_new(nullptr, nullptr);
        mpg123_open_feed(mpg);
        mpg123_format(mpg, SAMPLE_RATE, CHANNELS, MPG123_ENC_SIGNED_16);
        
        // Initialize curl
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        
        // Start playback thread
        playThread = std::thread(&AudioPlayer::playbackThread, this);
    }
    
    ~AudioPlayer() {
        shouldStop = true;
        if (playThread.joinable()) {
            playThread.join();
        }
        
        if (curl) {
            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();
        
        if (mpg) {
            mpg123_close(mpg);
            mpg123_delete(mpg);
        }
        mpg123_exit();
        
        for (int i = 0; i < 2; i++) {
            free(buf_data[i]);
        }
        
        audoutStopAudioOut();
        audoutExit();
    }
    
    void loadUrl(const std::string& url) {
        playing = false;
        currentUrl = url;
        
        // Reset mpg123
        mpg123_close(mpg);
        mpg123_open_feed(mpg);
        
        // Setup curl for streaming
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        
        // Start streaming in background
        std::thread([this]() {
            curl_easy_perform(curl);
        }).detach();
    }
    
    void play() {
        playing = true;
    }
    
    void pause() {
        playing = false;
    }
    
    void setVolume(float vol) {
        volume = std::min(1.0f, std::max(0.0f, vol));
    }
    
    bool isPlaying() const {
        return playing;
    }
};