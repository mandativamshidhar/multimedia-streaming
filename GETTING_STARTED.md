# Getting Started Guide

## Overview

This Real-Time Multimedia Streaming System is a professional-grade C++ library for:
- **Live video streaming** with H.264 codec support
- **Live audio streaming** with AAC codec support
- **Low-latency communication** using TCP/UDP networking
- **Multi-threaded architecture** for smooth performance
- **Cross-platform support** (Windows, Linux, macOS)

---

## Installation & Setup

### Prerequisites

- **CMake 3.10+** - Build system
- **Visual Studio 2022** or **GCC/Clang** - C++ compiler
- **C++17 support** - Modern C++ features

### Build Instructions

#### On Windows

```powershell
# 1. Clone or download the project
cd C:\Users\manda\Git_projects\Project

# 2. Create build directory
mkdir build
cd build

# 3. Configure with CMake
cmake ..

# 4. Build
cmake --build . --config Release

# 5. Run
.\bin\Release\streaming_app.exe
```

#### On Linux/macOS

```bash
# 1. Install dependencies
# Ubuntu/Debian:
sudo apt-get install cmake build-essential

# macOS:
brew install cmake

# 2. Build
cd ~/Git_projects/Project
mkdir -p build
cd build
cmake ..
cmake --build . --config Release

# 3. Run
./bin/streaming_app
```

---

## Running the Application

### Basic Run (Demo with Receiver + Sender)

```powershell
C:\Users\manda\Git_projects\Project\build\bin\streaming_app.exe
```

**Output:**
```
=== Real-Time Multimedia Streaming System ===
H.264/AAC Codec with TCP/UDP Networking
==========================================

[MAIN] Starting RECEIVER...
NetworkManager initialized as server (TCP:5000, UDP:5001)
StreamingEngine initialized in RECEIVER mode
[RECEIVER] Ready and listening on port 5000 (TCP) and 5001 (UDP)

[MAIN] Starting SENDER...
[SENDER] Connected!
[SENDER] Video frame 0 encoded and sent
[SENDER] Audio frame 0 encoded and sent
...
=== Application completed successfully ===
```

---

## Using the Library

### 1. Include in Your Project

```cpp
#include "streaming/engine.h"
using namespace multimedia;
```

### 2. Create a Receiver

```cpp
// Create receiver instance
streaming::StreamingEngine receiver(streaming::StreamingMode::RECEIVER);

// Start listening
receiver.initialize("0.0.0.0", 5000, 5001);

// Configure media
receiver.configureVideo(1920, 1080, 30, 5000000);
receiver.configureAudio(44100, 2, 128000);

// Receive frames
while (true) {
    auto frame = receiver.receiveVideoFrame(100);  // 100ms timeout
    if (frame) {
        // Process frame
        process_video(frame->getData());
    }
}
```

### 3. Create a Sender

```cpp
// Create sender instance
streaming::StreamingEngine sender(streaming::StreamingMode::SENDER);

// Connect to receiver
sender.initialize("192.168.1.100", 5000, 5001);

// Configure media
sender.configureVideo(1920, 1080, 30, 5000000);
sender.configureAudio(44100, 2, 128000);

// Send frames
std::vector<uint8_t> videoData = capture_video();  // From camera
sender.sendVideoFrame(videoData.data(), videoData.size());

std::vector<int16_t> audioData = capture_audio();  // From mic
sender.sendAudioFrame(audioData.data(), audioData.size());
```

---

## Common Use Cases

### Use Case 1: Live Streaming Server

```cpp
// Start receiver (server)
streaming::StreamingEngine server(streaming::StreamingMode::RECEIVER);
server.initialize("0.0.0.0", 5000, 5001);  // Listen on all interfaces
server.configureVideo(1920, 1080, 30, 5000000);
server.configureAudio(48000, 2, 192000);

// Multiple senders can connect (requires multi-client support)
while (application_running) {
    auto frame = server.receiveVideoFrame(1000);
    if (frame) {
        broadcast_to_clients(frame);
    }
}
```

### Use Case 2: Video Conferencing

```cpp
// User A (Receiver role)
streaming::StreamingEngine userA(streaming::StreamingMode::RECEIVER);
userA.initialize("0.0.0.0", 5000, 5001);
userA.configureVideo(1280, 720, 30, 2500000);
userA.configureAudio(44100, 2, 128000);

// User B (Sender role)
streaming::StreamingEngine userB(streaming::StreamingMode::SENDER);
userB.initialize("192.168.1.50", 5000, 5001);  // Connect to User A's IP
userB.configureVideo(1280, 720, 30, 2500000);
userB.configureAudio(44100, 2, 128000);

// Two-way communication needs both to be senders AND receivers
// (Current implementation supports one-directional)
```

### Use Case 3: Real-Time Monitoring

```cpp
// Camera/Sensor as SENDER
streaming::StreamingEngine camera(streaming::StreamingMode::SENDER);
camera.initialize("monitoring-server.local", 5000, 5001);
camera.configureVideo(1280, 720, 15, 1500000);  // 15 FPS, 1.5 Mbps

// Continuous streaming
while (camera_running) {
    auto frame = get_camera_frame();
    camera.sendVideoFrame(frame.data(), frame.size());
    std::this_thread::sleep_for(std::chrono::milliseconds(67));  // ~15 FPS
}
```

---

## Configuration Presets

### Video Quality Levels

```cpp
// SD Quality (480p)
engine.configureVideo(720, 480, 30, 1000000);

// HD Quality (720p)
engine.configureVideo(1280, 720, 30, 2500000);

// Full HD (1080p)
engine.configureVideo(1920, 1080, 30, 5000000);

// 4K UltraHD
engine.configureVideo(3840, 2160, 30, 15000000);
```

### Audio Quality Levels

```cpp
// Voice Communication (low bandwidth)
engine.configureAudio(8000, 1, 32000);     // 8kHz, mono, 32kbps

// Standard Quality
engine.configureAudio(44100, 2, 128000);   // 44.1kHz, stereo, 128kbps

// High Quality
engine.configureAudio(48000, 2, 192000);   // 48kHz, stereo, 192kbps
```

---

## Network Configuration

### Local Network (LAN)

```cpp
// Receiver
receiver.initialize("192.168.1.100", 5000, 5001);

// Sender
sender.initialize("192.168.1.100", 5000, 5001);
```

### Over Internet

```cpp
// Receiver (public IP)
receiver.initialize("0.0.0.0", 5000, 5001);  // Listen on all interfaces

// Sender (from anywhere)
sender.initialize("203.0.113.42", 5000, 5001);  // Public IP of receiver
```

### Firewall Rules

Allow inbound/outbound traffic:
- **TCP Port 5000** - Primary data transfer
- **UDP Port 5001** - Real-time streaming

---

## Monitoring & Statistics

```cpp
// Get real-time statistics
streaming::StreamingEngine::Stats stats = engine.getStatistics();

std::cout << "Frames Sent: " << stats.framesSent << std::endl;
std::cout << "Frames Received: " << stats.framesReceived << std::endl;
std::cout << "Bytes Received: " << stats.bytesReceived << std::endl;

// Calculate bandwidth
double bandwidth_mbps = (stats.bytesReceived * 8.0) / (1000000.0 * elapsed_seconds);
std::cout << "Bandwidth: " << bandwidth_mbps << " Mbps" << std::endl;

// Calculate frame rate
double frame_rate = stats.framesReceived / elapsed_seconds;
std::cout << "Frame Rate: " << frame_rate << " fps" << std::endl;
```

---

## Troubleshooting

### "Failed to create socket"
**Cause:** Winsock not initialized (Windows only)
**Solution:** Ensure Windows Socket API is properly linked

### "Bind failed on port 5000"
**Cause:** Port already in use
**Solution:** 
```powershell
# Windows: Find process using port
netstat -ano | findstr :5000

# Linux: Find process using port
sudo lsof -i :5000
```

### "Connect failed to server"
**Cause:** Server not running or IP/port incorrect
**Solution:**
- Verify server is listening
- Check IP address and ports
- Verify firewall allows connections

### Low frame rate or dropped frames
**Cause:** Network congestion or insufficient bandwidth
**Solution:**
- Reduce video resolution
- Lower frame rate
- Reduce bitrate
- Check network conditions

---

## File Structure

```
Project/
├── CMakeLists.txt                 # Build configuration
├── README.md                       # Project overview
├── USAGE_GUIDE.md                 # Detailed usage guide
├── examples.cpp                    # Code examples
├── src/
│   ├── main.cpp                   # Demo application
│   ├── networking/
│   │   ├── socket.cpp             # Socket implementation
│   │   └── network_manager.cpp    # Network management
│   ├── codec/
│   │   ├── frame.cpp
│   │   ├── h264_encoder.cpp
│   │   └── aac_encoder.cpp
│   └── streaming/
│       ├── buffer.cpp
│       └── engine.cpp
├── include/                        # Header files
│   ├── networking/
│   ├── codec/
│   └── streaming/
└── build/                          # Build output
    ├── bin/
    │   └── streaming_app.exe
    ├── lib/
    └── CMakeFiles/
```

---

## Next Steps

1. **Run the demo** to see it working
2. **Review examples.cpp** for code samples
3. **Integrate into your project** using the API
4. **Configure for your use case** (resolution, bitrate, etc.)
5. **Test with real video/audio sources**

---

## Support

- **Documentation:** See USAGE_GUIDE.md
- **Examples:** See examples.cpp
- **Issues:** Check troubleshooting section
- **Code:** Modify src/ and recompile with CMake

---

**Version:** 1.0.0  
**Last Updated:** December 27, 2025
