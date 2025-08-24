#pragma once
#include <switch.h>
#include <cstring>
#include <cmath>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>

class AudioManager {
private:
    static constexpr u32 SAMPLE_RATE = 48000;
    static constexpr u32 CHANNEL_COUNT = 2;
    static constexpr u32 BUFFER_COUNT = 2;
    static constexpr u32 BUFFER_SIZE = 4096 * CHANNEL_COUNT;
    
    AudioOutBuffer audioBuffers[BUFFER_COUNT];
    s16* bufferData[BUFFER_COUNT];
    u32 currentBuffer = 0;
    
    std::thread audioThread;
    std::atomic<bool> isPlaying{false};
    std::atomic<bool> shouldStop{false};
    std::atomic<float> volume{0.3f};
    
    // Current audio data
    std::vector<s16> audioData;
    size_t playbackPosition = 0;
    std::mutex audioMutex;
    
    void audioThreadFunc() {
        while (!shouldStop) {
            if (isPlaying && !audioData.empty()) {
                std::lock_guard<std::mutex> lock(audioMutex);
                
                // Fill current buffer
                s16* buffer = bufferData[currentBuffer];
                size_t samplesAvailable = audioData.size() - playbackPosition;
                size_t samplesToWrite = std::min((size_t)BUFFER_SIZE, samplesAvailable);
                
                if (samplesToWrite > 0) {
                    // Copy and apply volume
                    for (size_t i = 0; i < samplesToWrite; i++) {
                        if (playbackPosition < audioData.size()) {
                            buffer[i] = audioData[playbackPosition++] * volume;
                        } else {
                            buffer[i] = 0;
                        }
                    }
                    
                    // Fill rest with silence if needed
                    for (size_t i = samplesToWrite; i < BUFFER_SIZE; i++) {
                        buffer[i] = 0;
                    }
                    
                    // Submit buffer
                    audioBuffers[currentBuffer].data_size = BUFFER_SIZE * sizeof(s16);
                    audoutAppendAudioOutBuffer(&audioBuffers[currentBuffer]);
                    
                    // Wait for playback
                    AudioOutBuffer* released;
                    u32 releasedCount;
                    audoutWaitPlayFinish(&released, &releasedCount, UINT64_MAX);
                    
                    // Switch buffers
                    currentBuffer = (currentBuffer + 1) % BUFFER_COUNT;
                    
                    // Loop if reached end
                    if (playbackPosition >= audioData.size()) {
                        playbackPosition = 0;
                    }
                } else {
                    svcSleepThread(10000000); // 10ms
                }
            } else {
                svcSleepThread(50000000); // 50ms when not playing
            }
        }
    }
    
public:
    AudioManager() {
        // Initialize audio
        audoutInitialize();
        audoutStartAudioOut();
        
        // Allocate buffers
        for (u32 i = 0; i < BUFFER_COUNT; i++) {
            bufferData[i] = (s16*)memalign(0x1000, BUFFER_SIZE * sizeof(s16));
            memset(bufferData[i], 0, BUFFER_SIZE * sizeof(s16));
            
            audioBuffers[i].next = nullptr;
            audioBuffers[i].buffer = bufferData[i];
            audioBuffers[i].buffer_size = BUFFER_SIZE * sizeof(s16);
            audioBuffers[i].data_size = BUFFER_SIZE * sizeof(s16);
            audioBuffers[i].data_offset = 0;
        }
        
        // Start audio thread
        audioThread = std::thread(&AudioManager::audioThreadFunc, this);
    }
    
    ~AudioManager() {
        stop();
        shouldStop = true;
        if (audioThread.joinable()) {
            audioThread.join();
        }
        
        for (u32 i = 0; i < BUFFER_COUNT; i++) {
            if (bufferData[i]) {
                free(bufferData[i]);
            }
        }
        
        audoutStopAudioOut();
        audoutExit();
    }
    
    void loadTestTone(float frequency = 440.0f, float duration = 3.0f) {
        std::lock_guard<std::mutex> lock(audioMutex);
        
        audioData.clear();
        u32 totalSamples = SAMPLE_RATE * duration * CHANNEL_COUNT;
        audioData.reserve(totalSamples);
        
        for (u32 i = 0; i < totalSamples; i += CHANNEL_COUNT) {
            float t = (float)(i / CHANNEL_COUNT) / SAMPLE_RATE;
            s16 sample = (s16)(32767.0f * 0.3f * sinf(2.0f * M_PI * frequency * t));
            
            // Stereo
            audioData.push_back(sample);
            audioData.push_back(sample);
        }
        
        playbackPosition = 0;
    }
    
    void loadMelody() {
        std::lock_guard<std::mutex> lock(audioMutex);
        
        audioData.clear();
        
        // Simple melody - Mario coin sound style
        float notes[] = {523.25f, 659.25f, 783.99f, 1046.50f}; // C5, E5, G5, C6
        float durations[] = {0.1f, 0.1f, 0.1f, 0.3f};
        
        for (int note = 0; note < 4; note++) {
            u32 noteSamples = SAMPLE_RATE * durations[note] * CHANNEL_COUNT;
            
            for (u32 i = 0; i < noteSamples; i += CHANNEL_COUNT) {
                float t = (float)(i / CHANNEL_COUNT) / SAMPLE_RATE;
                s16 sample = (s16)(32767.0f * 0.2f * sinf(2.0f * M_PI * notes[note] * t));
                
                // Apply envelope for smoother sound
                float envelope = 1.0f;
                if (i < 1000) envelope = i / 1000.0f;
                if (i > noteSamples - 1000) envelope = (noteSamples - i) / 1000.0f;
                sample *= envelope;
                
                audioData.push_back(sample);
                audioData.push_back(sample);
            }
            
            // Small gap between notes
            for (u32 i = 0; i < 480; i++) {
                audioData.push_back(0);
                audioData.push_back(0);
            }
        }
        
        playbackPosition = 0;
    }
    
    void play() {
        isPlaying = true;
    }
    
    void pause() {
        isPlaying = false;
    }
    
    void stop() {
        isPlaying = false;
        std::lock_guard<std::mutex> lock(audioMutex);
        playbackPosition = 0;
    }
    
    void setVolume(float vol) {
        volume = std::max(0.0f, std::min(1.0f, vol));
    }
    
    float getVolume() const {
        return volume;
    }
    
    bool getIsPlaying() const {
        return isPlaying;
    }
    
    float getProgress() const {
        if (audioData.empty()) return 0.0f;
        return (float)playbackPosition / audioData.size();
    }
};