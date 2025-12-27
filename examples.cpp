#!/usr/bin/env cpp
/**
 * Quick Start Example - Multimedia Streaming System
 * 
 * This example demonstrates how to use the streaming library
 * to send and receive video/audio frames.
 */

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "streaming/engine.h"

using namespace multimedia;

// Example 1: Simple RECEIVER
void example_receiver() {
    std::cout << "=== RECEIVER Example ===" << std::endl;
    
    streaming::StreamingEngine receiver(streaming::StreamingMode::RECEIVER);
    
    // Listen on localhost:5000 (TCP) and 5001 (UDP)
    if (!receiver.initialize("0.0.0.0", 5000, 5001)) {
        std::cerr << "Failed to initialize receiver" << std::endl;
        return;
    }
    
    receiver.configureVideo(1920, 1080, 30, 5000000);
    receiver.configureAudio(44100, 2, 128000);
    
    std::cout << "Receiver listening on port 5000..." << std::endl;
    
    // Receive 10 frames
    for (int i = 0; i < 10; ++i) {
        auto frame = receiver.receiveVideoFrame(1000);  // 1 second timeout
        if (frame) {
            std::cout << "Received frame " << i << ": " 
                      << frame->getSize() << " bytes, "
                      << "sequence: " << frame->getSequenceNumber() << std::endl;
        }
    }
    
    auto stats = receiver.getStatistics();
    std::cout << "Total frames received: " << stats.framesReceived << std::endl;
}

// Example 2: Simple SENDER
void example_sender() {
    std::cout << "=== SENDER Example ===" << std::endl;
    
    streaming::StreamingEngine sender(streaming::StreamingMode::SENDER);
    
    // Connect to receiver at localhost:5000 (TCP) and 5001 (UDP)
    if (!sender.initialize("127.0.0.1", 5000, 5001)) {
        std::cerr << "Failed to initialize sender" << std::endl;
        return;
    }
    
    sender.configureVideo(1920, 1080, 30, 5000000);
    sender.configureAudio(44100, 2, 128000);
    
    std::cout << "Sender connected to receiver" << std::endl;
    
    // Send 10 video frames
    for (int i = 0; i < 10; ++i) {
        // Create dummy video frame (1920x1080 RGB)
        std::vector<uint8_t> videoData(1920 * 1080 * 3, i * 25 % 256);
        
        if (sender.sendVideoFrame(videoData.data(), videoData.size())) {
            std::cout << "Sent video frame " << i << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(33));  // ~30 FPS
    }
    
    auto stats = sender.getStatistics();
    std::cout << "Total frames sent: " << stats.framesSent << std::endl;
}

// Example 3: Sender and Receiver in Separate Threads
void example_threaded() {
    std::cout << "=== Threaded Sender/Receiver Example ===" << std::endl;
    
    // Receiver thread
    std::thread receiver_thread([]() {
        streaming::StreamingEngine receiver(streaming::StreamingMode::RECEIVER);
        receiver.initialize("0.0.0.0", 6000, 6001);
        receiver.configureVideo(1280, 720, 30, 2500000);
        receiver.configureAudio(44100, 2, 128000);
        
        std::cout << "[RECEIVER] Listening..." << std::endl;
        
        for (int i = 0; i < 5; ++i) {
            auto frame = receiver.receiveVideoFrame(500);
            if (frame) {
                std::cout << "[RECEIVER] Got frame " << frame->getSequenceNumber() << std::endl;
            }
        }
    });
    
    // Give receiver time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Sender thread
    std::thread sender_thread([]() {
        streaming::StreamingEngine sender(streaming::StreamingMode::SENDER);
        sender.initialize("127.0.0.1", 6000, 6001);
        sender.configureVideo(1280, 720, 30, 2500000);
        sender.configureAudio(44100, 2, 128000);
        
        std::cout << "[SENDER] Connected!" << std::endl;
        
        for (int i = 0; i < 5; ++i) {
            std::vector<uint8_t> frame(1280 * 720 * 3, i * 50);
            sender.sendVideoFrame(frame.data(), frame.size());
            std::cout << "[SENDER] Sent frame " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
    
    receiver_thread.join();
    sender_thread.join();
    
    std::cout << "[MAIN] Both threads completed" << std::endl;
}

// Example 4: Video Conference Simulation
void example_video_conference() {
    std::cout << "=== Video Conference Example ===" << std::endl;
    
    // User 1 (Receiver role)
    streaming::StreamingEngine user1(streaming::StreamingMode::RECEIVER);
    user1.initialize("0.0.0.0", 7000, 7001);
    user1.configureVideo(640, 480, 24, 1500000);
    user1.configureAudio(16000, 1, 64000);
    
    std::cout << "User 1 ready on port 7000" << std::endl;
    
    // User 2 (Sender role) - in separate thread
    std::thread user2_thread([&user1]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        streaming::StreamingEngine user2(streaming::StreamingMode::SENDER);
        user2.initialize("127.0.0.1", 7000, 7001);
        user2.configureVideo(640, 480, 24, 1500000);
        user2.configureAudio(16000, 1, 64000);
        
        std::cout << "User 2 connected" << std::endl;
        
        // Simulate video conference (30 seconds at 24 FPS = 720 frames)
        for (int i = 0; i < 30; ++i) {
            // Send video (VGA quality)
            std::vector<uint8_t> video(640 * 480 * 3);
            // In real app, fill with camera data
            for (auto& pixel : video) pixel = (i * 8) % 256;
            
            user2.sendVideoFrame(video.data(), video.size());
            
            // Send audio every second
            if (i % 24 == 0) {
                std::vector<int16_t> audio(16000);
                user2.sendAudioFrame(audio.data(), audio.size());
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(42));  // 24 FPS
        }
        
        auto stats = user2.getStatistics();
        std::cout << "User 2 sent " << stats.framesSent << " frames" << std::endl;
    });
    
    // User 1 receives for 30 seconds
    int frames_received = 0;
    auto start = std::chrono::steady_clock::now();
    
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(30)) {
        auto frame = user1.receiveVideoFrame(50);
        if (frame) frames_received++;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    user2_thread.join();
    
    std::cout << "User 1 received " << frames_received << " frames" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Usage: examples <example_number>" << std::endl;
        std::cout << "  1 - Receiver example" << std::endl;
        std::cout << "  2 - Sender example" << std::endl;
        std::cout << "  3 - Threaded sender/receiver" << std::endl;
        std::cout << "  4 - Video conference simulation" << std::endl;
        return 1;
    }
    
    int example = std::stoi(argv[1]);
    
    switch (example) {
        case 1:
            example_receiver();
            break;
        case 2:
            example_sender();
            break;
        case 3:
            example_threaded();
            break;
        case 4:
            example_video_conference();
            break;
        default:
            std::cerr << "Unknown example: " << example << std::endl;
            return 1;
    }
    
    return 0;
}
