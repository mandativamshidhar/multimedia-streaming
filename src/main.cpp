#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include "streaming/engine.h"

using namespace multimedia;

// Demo: Simulate video frame capture
void simulateVideoCapture(streaming::StreamingEngine& engine) {
    std::cout << "[SENDER] Starting video capture simulation..." << std::endl;
    
    std::vector<uint8_t> dummyFrame(1920 * 1080 * 3, 0x42); // RGB data with pattern
    
    for (int i = 0; i < 10; ++i) {
        if (engine.sendVideoFrame(dummyFrame.data(), dummyFrame.size())) {
            std::cout << "[SENDER] Video frame " << i << " encoded and sent" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(33)); // ~30 FPS
    }
}

// Demo: Simulate audio frame capture
void simulateAudioCapture(streaming::StreamingEngine& engine) {
    std::cout << "[SENDER] Starting audio capture simulation..." << std::endl;
    
    std::vector<int16_t> dummyAudio(44100, 0); // 1 second of audio at 44.1kHz
    
    for (int i = 0; i < 5; ++i) {
        if (engine.sendAudioFrame(dummyAudio.data(), dummyAudio.size())) {
            std::cout << "[SENDER] Audio frame " << i << " encoded and sent" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// Demo: Simulate receiving frames
void simulateReceive(streaming::StreamingEngine& engine) {
    std::cout << "[RECEIVER] Starting receive simulation..." << std::endl;
    
    for (int i = 0; i < 20; ++i) {
        auto videoFrame = engine.receiveVideoFrame(100);
        if (videoFrame) {
            std::cout << "[RECEIVER] Received video frame: " << videoFrame->getSize() 
                      << " bytes (seq: " << videoFrame->getSequenceNumber() << ")" << std::endl;
        }
        
        auto audioFrame = engine.receiveAudioFrame(100);
        if (audioFrame) {
            std::cout << "[RECEIVER] Received audio frame: " << audioFrame->getSize() << " bytes" << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main()
{
    std::cout << "=== Real-Time Multimedia Streaming System ===" << std::endl;
    std::cout << "H.264/AAC Codec with TCP/UDP Networking" << std::endl;
    std::cout << "==========================================" << std::endl << std::endl;

    try {
        // Create receiver thread first
        std::cout << "[MAIN] Starting RECEIVER..." << std::endl;
        streaming::StreamingEngine receiver(streaming::StreamingMode::RECEIVER);
        
        if (!receiver.initialize("127.0.0.1", 5000, 5001)) {
            std::cerr << "[ERROR] Failed to initialize receiver" << std::endl;
            return 1;
        }
        
        // Configure receiver
        receiver.configureVideo(1920, 1080, 30, 5000000);
        receiver.configureAudio(44100, 2, 128000);
        
        std::cout << "[RECEIVER] Ready and listening on port 5000 (TCP) and 5001 (UDP)" << std::endl;
        std::cout << std::endl;
        
        // Give receiver time to start listening
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Create sender in separate thread
        std::cout << "[MAIN] Starting SENDER..." << std::endl;
        std::thread senderThread([&receiver]() {
            try {
                streaming::StreamingEngine sender(streaming::StreamingMode::SENDER);
                
                // Initialize with localhost
                if (!sender.initialize("127.0.0.1", 5000, 5001)) {
                    std::cerr << "[ERROR] Failed to initialize sender" << std::endl;
                    return;
                }
                
                // Configure video: 1920x1080 @ 30 FPS, 5 Mbps bitrate
                if (!sender.configureVideo(1920, 1080, 30, 5000000)) {
                    std::cerr << "[ERROR] Failed to configure video" << std::endl;
                    return;
                }
                
                // Configure audio: 44.1kHz, stereo, 128 kbps
                if (!sender.configureAudio(44100, 2, 128000)) {
                    std::cerr << "[ERROR] Failed to configure audio" << std::endl;
                    return;
                }
                
                std::cout << "[SENDER] Configuration:" << std::endl;
                std::cout << "[SENDER]   Video: 1920x1080 @ 30 FPS, 5 Mbps" << std::endl;
                std::cout << "[SENDER]   Audio: 44.1kHz, 2 channels, 128 kbps" << std::endl << std::endl;
                
                // Simulate video and audio capture
                std::thread videoThread(simulateVideoCapture, std::ref(sender));
                std::thread audioThread(simulateAudioCapture, std::ref(sender));
                
                videoThread.join();
                audioThread.join();
                
                // Print sender statistics
                auto stats = sender.getStatistics();
                std::cout << "\n[SENDER] --- Streaming Statistics ---" << std::endl;
                std::cout << "[SENDER] Frames Encoded: " << stats.framesEncoded << std::endl;
                std::cout << "[SENDER] Frames Sent: " << stats.framesSent << std::endl;
                std::cout << "[SENDER] Frames Received: " << stats.framesReceived << std::endl;
                std::cout << "[SENDER] Bytes Received: " << stats.bytesReceived << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "[SENDER] Exception: " << e.what() << std::endl;
            }
        });
        
        // Receiver receives in main thread
        simulateReceive(receiver);
        
        // Wait for sender thread to complete
        senderThread.join();
        
        // Print receiver statistics
        auto stats = receiver.getStatistics();
        std::cout << "\n[RECEIVER] --- Streaming Statistics ---" << std::endl;
        std::cout << "[RECEIVER] Frames Encoded: " << stats.framesEncoded << std::endl;
        std::cout << "[RECEIVER] Frames Sent: " << stats.framesSent << std::endl;
        std::cout << "[RECEIVER] Frames Received: " << stats.framesReceived << std::endl;
        std::cout << "[RECEIVER] Bytes Received: " << stats.bytesReceived << std::endl;
        
        std::cout << "\n=== Application completed successfully ===" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
