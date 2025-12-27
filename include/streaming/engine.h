#ifndef STREAMING_ENGINE_H
#define STREAMING_ENGINE_H

#include "buffer.h"
#include "networking/network_manager.h"
#include "codec/h264_encoder.h"
#include "codec/aac_encoder.h"
#include <thread>
#include <atomic>
#include <chrono>

namespace multimedia {
namespace streaming {

enum class StreamingMode {
    SENDER,
    RECEIVER
};

class StreamingEngine {
public:
    StreamingEngine(StreamingMode mode);
    ~StreamingEngine();

    // Initialize streaming
    bool initialize(const std::string& serverAddr, int tcpPort, int udpPort);
    
    // Video operations
    bool configureVideo(int width, int height, int fps, int bitrate);
    bool sendVideoFrame(const uint8_t* rgbData, size_t dataSize);
    std::shared_ptr<codec::Frame> receiveVideoFrame(int timeoutMs = 100);
    
    // Audio operations
    bool configureAudio(int sampleRate, int channels, int bitrate);
    bool sendAudioFrame(const int16_t* audioData, size_t sampleCount);
    std::shared_ptr<codec::Frame> receiveAudioFrame(int timeoutMs = 100);
    
    // Status
    bool isRunning() const { return running_; }
    bool isConnected() const;
    
    // Statistics
    struct Stats {
        uint64_t framesEncoded;
        uint64_t framesSent;
        uint64_t framesReceived;
        uint64_t bytesReceived;
    };
    Stats getStatistics() const;

private:
    StreamingMode mode_;
    std::unique_ptr<networking::NetworkManager> networkManager_;
    std::unique_ptr<codec::H264Encoder> h264Encoder_;
    std::unique_ptr<codec::AACEncoder> aacEncoder_;
    
    FrameBuffer videoBuffer_;
    FrameBuffer audioBuffer_;
    
    std::thread receiveThread_;
    std::thread encodeThread_;
    std::atomic<bool> running_;
    
    // Statistics
    std::atomic<uint64_t> framesEncoded_{0};
    std::atomic<uint64_t> framesSent_{0};
    std::atomic<uint64_t> framesReceived_{0};
    std::atomic<uint64_t> bytesReceived_{0};
    
    void receiveThreadFunction();
    void encodeThreadFunction();
};

} // namespace streaming
} // namespace multimedia

#endif // STREAMING_ENGINE_H
