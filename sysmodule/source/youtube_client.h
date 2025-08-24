#pragma once
#include <string>
#include <vector>
#include <curl/curl.h>
#include <json-c/json.h>

struct TrackInfo {
    std::string title;
    std::string artist;
    std::string url;
    std::string videoId;
    int duration;
};

class YouTubeClient {
private:
    // Using Invidious API (self-hosted YouTube proxy)
    std::string INVIDIOUS_INSTANCE = "https://invidious.fdn.fr";
    
    std::vector<TrackInfo> playlist;
    size_t currentIndex = 0;
    CURL* curl = nullptr;
    
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
        userp->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
    
    std::string httpGet(const std::string& url) {
        std::string response;
        
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
            
            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                return "";
            }
        }
        
        return response;
    }
    
public:
    YouTubeClient() {
        curl = curl_easy_init();
    }
    
    ~YouTubeClient() {
        if (curl) {
            curl_easy_cleanup(curl);
        }
    }
    
    std::vector<TrackInfo> search(const std::string& query) {
        playlist.clear();
        currentIndex = 0;
        
        // URL encode query
        char* encoded = curl_easy_escape(curl, query.c_str(), query.length());
        std::string url = INVIDIOUS_INSTANCE + "/api/v1/search?q=" + encoded + "&type=video";
        curl_free(encoded);
        
        std::string response = httpGet(url);
        if (response.empty()) return playlist;
        
        // Parse JSON response
        json_object* root = json_tokener_parse(response.c_str());
        if (!root) return playlist;
        
        int arrayLen = json_object_array_length(root);
        for (int i = 0; i < std::min(20, arrayLen); i++) {
            json_object* item = json_object_array_get_idx(root, i);
            
            TrackInfo track;
            
            json_object* title_obj;
            if (json_object_object_get_ex(item, "title", &title_obj)) {
                track.title = json_object_get_string(title_obj);
            }
            
            json_object* author_obj;
            if (json_object_object_get_ex(item, "author", &author_obj)) {
                track.artist = json_object_get_string(author_obj);
            }
            
            json_object* videoId_obj;
            if (json_object_object_get_ex(item, "videoId", &videoId_obj)) {
                track.videoId = json_object_get_string(videoId_obj);
                track.url = "https://youtube.com/watch?v=" + track.videoId;
            }
            
            json_object* length_obj;
            if (json_object_object_get_ex(item, "lengthSeconds", &length_obj)) {
                track.duration = json_object_get_int(length_obj);
            }
            
            if (!track.videoId.empty()) {
                playlist.push_back(track);
            }
        }
        
        json_object_put(root);
        return playlist;
    }
    
    std::string getStreamUrl(const std::string& videoUrl) {
        // Extract video ID from URL
        std::string videoId;
        size_t pos = videoUrl.find("v=");
        if (pos != std::string::npos) {
            videoId = videoUrl.substr(pos + 2, 11);
        } else if (videoUrl.find("youtu.be/") != std::string::npos) {
            pos = videoUrl.find("youtu.be/");
            videoId = videoUrl.substr(pos + 9, 11);
        } else {
            videoId = videoUrl; // Assume it's already a video ID
        }
        
        // Get video info from Invidious
        std::string url = INVIDIOUS_INSTANCE + "/api/v1/videos/" + videoId;
        std::string response = httpGet(url);
        
        if (response.empty()) return "";
        
        json_object* root = json_tokener_parse(response.c_str());
        if (!root) return "";
        
        std::string streamUrl;
        
        // Look for audio streams
        json_object* adaptiveFormats;
        if (json_object_object_get_ex(root, "adaptiveFormats", &adaptiveFormats)) {
            int arrayLen = json_object_array_length(adaptiveFormats);
            
            for (int i = 0; i < arrayLen; i++) {
                json_object* format = json_object_array_get_idx(adaptiveFormats, i);
                
                json_object* type_obj;
                if (json_object_object_get_ex(format, "type", &type_obj)) {
                    std::string type = json_object_get_string(type_obj);
                    
                    // Look for audio formats
                    if (type.find("audio/mp4") != std::string::npos ||
                        type.find("audio/webm") != std::string::npos) {
                        
                        json_object* url_obj;
                        if (json_object_object_get_ex(format, "url", &url_obj)) {
                            streamUrl = json_object_get_string(url_obj);
                            break;
                        }
                    }
                }
            }
        }
        
        json_object_put(root);
        return streamUrl;
    }
    
    void setPlaylist(const std::vector<TrackInfo>& tracks) {
        playlist = tracks;
        currentIndex = 0;
    }
    
    TrackInfo getCurrentTrack() {
        if (currentIndex < playlist.size()) {
            return playlist[currentIndex];
        }
        return TrackInfo{};
    }
    
    TrackInfo getCurrentInfo() {
        return getCurrentTrack();
    }
    
    void nextTrack() {
        if (currentIndex < playlist.size() - 1) {
            currentIndex++;
        }
    }
    
    void previousTrack() {
        if (currentIndex > 0) {
            currentIndex--;
        }
    }
};