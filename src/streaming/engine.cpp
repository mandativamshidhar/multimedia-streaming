#include "streaming/engine.h"
#include <iostream>
#include <chrono>

namespace multimedia {
namespace streaming {

StreamingEngine::StreamingEngine(StreamingMode mode)
    : mode_(mode), running_(false), videoBuffer_(100), audioBuffer_(100)
{
    networkManager_ = std::make_unique<networking::NetworkManager>();
    h264Encoder_ = std::make_unique<codec::H264Encoder>();
    aacEncoder_ = std::make_unique<codec::AACEncoder>();
}

StreamingEngine::~StreamingEngine()
{
    running_ = false;
    if (receiveThread_.joinable()) {
        receiveThread_.join();
    }
    if (encodeThread_.joinable()) {
        encodeThread_.join();
    }
}

bool StreamingEngine::initialize(const std::string& serverAddr, int tcpPort, int udpPort)
{
    if (mode_ == StreamingMode::SENDER) {
        if (!networkManager_->initializeClient(serverAddr, tcpPort, udpPort)) {
            std::cerr << "Failed to initialize as sender" << std::endl;
            return false;
        }
    } else {
        if (!networkManager_->initializeServer(tcpPort, udpPort)) {
            std::cerr << "Failed to initialize as receiver" << std::endl;
            return false;
        }
    }
    
    running_ = true;
    receiveThread_ = std::thread(&StreamingEngine::receiveThreadFunction, this);
    
    std::cout << "StreamingEngine initialized in " 
              << (mode_ == StreamingMode::SENDER ? "SENDER" : "RECEIVER") << " mode" << std::endl;
    
    return true;
}

bool StreamingEngine::configureVideo(int width, int height, int fps, int bitrate)
{
    return h264Encoder_->configure(width, height, fps, bitrate);
}

bool StreamingEngine::sendVideoFrame(const uint8_t* rgbData, size_t dataSize)
{
    auto encodedFrames = h264Encoder_->encodeFrame(rgbData, dataSize);
    
    for (auto& frame : encodedFrames) {
        if (networkManager_->sendViaTCP(frame->getData())) {
            framesSent_++;
        }
    }
    
    return true;
}

std::shared_ptr<codec::Frame> StreamingEngine::receiveVideoFrame(int timeoutMs)
{
    auto startTime = std::chrono::steady_clock::now();
    
    while (std::chrono::steady_clock::now() - startTime < std::chrono::milliseconds(timeoutMs)) {
        std::shared_ptr<codec::Frame> frame;
        if (videoBuffer_.tryPop(frame)) {
            return frame;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    return nullptr;
}

bool StreamingEngine::configureAudio(int sampleRate, int channels, int bitrate)
{
    return aacEncoder_->configure(sampleRate, channels, bitrate);
}

bool StreamingEngine::sendAudioFrame(const int16_t* audioData, size_t sampleCount)
{
    auto frame = aacEncoder_->encodeFrame(audioData, sampleCount);
    
    if (frame && networkManager_->sendViaTCP(frame->getData())) {
        framesSent_++;
        return true;
    }
    
    return false;
}

std::shared_ptr<codec::Frame> StreamingEngine::receiveAudioFrame(int timeoutMs)
{
    auto startTime = std::chrono::steady_clock::now();
    
    while (std::chrono::steady_clock::now() - startTime < std::chrono::milliseconds(timeoutMs)) {
        std::shared_ptr<codec::Frame> frame;
        if (audioBuffer_.tryPop(frame)) {
            return frame;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    return nullptr;
}

bool StreamingEngine::isConnected() const
{
    return networkManager_->isConnected();
}

StreamingEngine::Stats StreamingEngine::getStatistics() const
{
    return {
        framesEncoded_.load(),
        framesSent_.load(),
        framesReceived_.load(),
        bytesReceived_.load()
    };
}

void StreamingEngine::receiveThreadFunction()
{
    while (running_) {
        if (networkManager_->hasPackets()) {
            auto packet = networkManager_->getNextPacket();
            
            // Decode frame (placeholder)
            auto frame = std::make_shared<codec::Frame>(codec::CodecType::H264_VIDEO);
            frame->setData(packet.data);
            frame->setTimestamp(packet.timestamp);
            
            if (!videoBuffer_.push(frame)) {
                // Buffer full, frame dropped
            } else {
                framesReceived_++;
                bytesReceived_ += packet.data.size();
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void StreamingEngine::encodeThreadFunction()
{
    // Encoding is done on-demand in this implementation
}

} // namespace streaming
} // namespace multimedia
