# Usage Guide - Real-Time Multimedia Streaming System

## Quick Start

### Running the Demo Application

```powershell
C:\Users\manda\Git_projects\Project\build\bin\streaming_app.exe
```

This runs a built-in demo with a receiver and sender communicating on localhost.

---

## Integration Guide

### 1. Include the Streaming Library

```cpp
#include "streaming/engine.h"

using namespace multimedia;
```

### 2. Create a Streaming Engine

```cpp
// For SENDER (sends video/audio)
streaming::StreamingEngine sender(streaming::StreamingMode::SENDER);

// For RECEIVER (receives video/audio)
streaming::StreamingEngine receiver(streaming::StreamingMode::RECEIVER);
```

### 3. Initialize the Connection

```cpp
// Receiver: Listen for incoming connections
bool success = receiver.initialize(
    "0.0.0.0",    // Listen on all interfaces
    5000,         // TCP port
    5001          // UDP port
);

// Sender: Connect to receiver
bool success = sender.initialize(
    "192.168.1.100",  // Server IP address
    5000,             // TCP port
    5001              // UDP port
);
```

### 4. Configure Video and Audio

```cpp
// Configure Video: resolution, FPS, bitrate
sender.configureVideo(
    1920,        // Width
    1080,        // Height
    30,          // Frames per second
    5000000      // Bitrate in bps (5 Mbps)
);

// Configure Audio: sample rate, channels, bitrate
sender.configureAudio(
    44100,       // Sample rate (Hz)
    2,           // Channels (1=mono, 2=stereo)
    128000       // Bitrate in bps (128 kbps)
);
```

### 5. Send Video Frames

```cpp
#include <vector>

// Assume rgbData is your raw RGB image data (3 bytes per pixel)
std::vector<uint8_t> videoFrame(1920 * 1080 * 3);
// Fill rgbData with your camera or video source...

bool sent = sender.sendVideoFrame(videoFrame.data(), videoFrame.size());
```

### 6. Send Audio Frames

```cpp
#include <vector>

// Assume audioData is your raw PCM audio data (16-bit samples)
std::vector<int16_t> audioFrame(44100);  // 1 second at 44.1kHz
// Fill audioFrame with your microphone or audio source...

bool sent = sender.sendAudioFrame(audioFrame.data(), audioFrame.size());
```

### 7. Receive Frames (Receiver Side)

```cpp
// Receive video frame (with 100ms timeout)
auto videoFrame = receiver.receiveVideoFrame(100);
if (videoFrame) {
    size_t frameSize = videoFrame->getSize();
    uint64_t timestamp = videoFrame->getTimestamp();
    auto frameData = videoFrame->getData();
    // Process frame...
}

// Receive audio frame
auto audioFrame = receiver.receiveAudioFrame(100);
if (audioFrame) {
    // Process audio frame...
}
```

---

## Complete Example: Simple Streaming Application

```cpp
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "streaming/engine.h"

using namespace multimedia;

int main() {
    // Create receiver
    streaming::StreamingEngine receiver(streaming::StreamingMode::RECEIVER);
    receiver.initialize("0.0.0.0", 5000, 5001);
    receiver.configureVideo(1920, 1080, 30, 5000000);
    receiver.configureAudio(44100, 2, 128000);
    
    std::cout << "Receiver listening on port 5000..." << std::endl;
    
    // Create sender in another thread
    std::thread senderThread([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        streaming::StreamingEngine sender(streaming::StreamingMode::SENDER);
        sender.initialize("127.0.0.1", 5000, 5001);
        sender.configureVideo(1920, 1080, 30, 5000000);
        sender.configureAudio(44100, 2, 128000);
        
        std::cout << "Sender connected!" << std::endl;
        
        // Send frames
        for (int i = 0; i < 30; ++i) {
            std::vector<uint8_t> videoData(1920 * 1080 * 3, rand() % 256);
            sender.sendVideoFrame(videoData.data(), videoData.size());
            std::this_thread::sleep_for(std::chrono::milliseconds(33)); // ~30 FPS
        }
    });
    
    // Receive frames
    for (int i = 0; i < 30; ++i) {
        auto frame = receiver.receiveVideoFrame(100);
        if (frame) {
            std::cout << "Received frame " << i << ": " 
                      << frame->getSize() << " bytes" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    senderThread.join();
    
    // Get statistics
    auto stats = receiver.getStatistics();
    std::cout << "Total frames received: " << stats.framesReceived << std::endl;
    std::cout << "Total bytes received: " << stats.bytesReceived << std::endl;
    
    return 0;
}
```

---

## API Reference

### StreamingEngine Class

#### Constructor
```cpp
StreamingEngine(StreamingMode mode);  // SENDER or RECEIVER
```

#### Methods

**Initialization:**
```cpp
bool initialize(const std::string& serverAddr, int tcpPort, int udpPort);
bool isConnected() const;
bool isRunning() const;
```

**Video Operations:**
```cpp
bool configureVideo(int width, int height, int fps, int bitrate);
bool sendVideoFrame(const uint8_t* rgbData, size_t dataSize);
std::shared_ptr<codec::Frame> receiveVideoFrame(int timeoutMs = 100);
```

**Audio Operations:**
```cpp
bool configureAudio(int sampleRate, int channels, int bitrate);
bool sendAudioFrame(const int16_t* audioData, size_t sampleCount);
std::shared_ptr<codec::Frame> receiveAudioFrame(int timeoutMs = 100);
```

**Statistics:**
```cpp
struct Stats {
    uint64_t framesEncoded;   // Total frames encoded
    uint64_t framesSent;       // Total frames sent
    uint64_t framesReceived;   // Total frames received
    uint64_t bytesReceived;    // Total bytes received
};

Stats getStatistics() const;
```

### Frame Class

```cpp
class Frame {
public:
    void setTimestamp(uint64_t ts);
    uint64_t getTimestamp() const;
    
    void setSequenceNumber(uint32_t seq);
    uint32_t getSequenceNumber() const;
    
    const std::vector<uint8_t>& getData() const;
    size_t getSize() const;
    
    CodecType getCodecType() const;  // H264_VIDEO or AAC_AUDIO
    FrameType getFrameType() const;  // I_FRAME, P_FRAME, or B_FRAME
    bool isKeyFrame() const;
};
```

---

## Configuration Parameters

### Video Presets

**SD (480p):**
```cpp
engine.configureVideo(720, 480, 30, 1000000);    // 1 Mbps
```

**HD (720p):**
```cpp
engine.configureVideo(1280, 720, 30, 2500000);   // 2.5 Mbps
```

**Full HD (1080p):**
```cpp
engine.configureVideo(1920, 1080, 30, 5000000);  // 5 Mbps
```

**4K:**
```cpp
engine.configureVideo(3840, 2160, 30, 15000000); // 15 Mbps
```

### Audio Presets

**Telephone Quality:**
```cpp
engine.configureAudio(8000, 1, 32000);     // 8kHz mono, 32kbps
```

**Voice Communication:**
```cpp
engine.configureAudio(16000, 1, 64000);    // 16kHz mono, 64kbps
```

**Standard Quality:**
```cpp
engine.configureAudio(44100, 2, 128000);   // 44.1kHz stereo, 128kbps
```

**High Quality:**
```cpp
engine.configureAudio(48000, 2, 192000);   // 48kHz stereo, 192kbps
```

---

## Building Custom Applications

### Step 1: Include Headers

```cpp
#include "streaming/engine.h"
#include "streaming/buffer.h"
#include "codec/frame.h"
#include "networking/network_manager.h"
```

### Step 2: Link Libraries

When building, link against:
- `ws2_32` (Windows Socket library on Windows)

### Step 3: Compile

```powershell
# Using the provided build system
cd build
cmake --build . --config Release
```

---

## Network Requirements

### Firewall Configuration

Allow traffic on these ports:
- **TCP Port 5000** - Control and reliable data
- **UDP Port 5001** - Real-time streaming

### Network Topology

```
┌─────────────────────┐
│  Sender (Client)    │
│  192.168.1.50:PORT  │
└──────────┬──────────┘
           │
        TCP/UDP
           │
┌──────────▼──────────┐
│ Receiver (Server)   │
│ 192.168.1.100:5000  │
└─────────────────────┘
```

### Bandwidth Requirements

| Resolution | FPS | Bitrate |
|-----------|-----|---------|
| 720p      | 30  | 2.5 Mbps|
| 1080p     | 30  | 5 Mbps  |
| 4K        | 30  | 15 Mbps |

---

## Troubleshooting

### Connection Issues

**Problem:** "Failed to connect"
- Check server is running
- Verify IP address and ports
- Check firewall settings

**Problem:** "Socket bind failed"
- Port already in use
- Try different port numbers
- Check permissions

### Performance Issues

**Problem:** Frame drops
- Increase buffer size: `buffer.setMaxSize(200)`
- Reduce bitrate
- Check network conditions

**Problem:** High latency
- Use UDP for real-time (lower latency)
- Reduce frame resolution
- Increase available bandwidth

---

## Advanced Topics

### Custom Frame Processing

```cpp
auto frame = receiver.receiveVideoFrame(100);
if (frame) {
    // Access raw frame data
    const auto& data = frame->getData();
    
    // Check frame type
    if (frame->isKeyFrame()) {
        // Handle keyframe (I-frame)
    }
    
    // Get timing information
    uint64_t timestamp = frame->getTimestamp();
    uint32_t sequence = frame->getSequenceNumber();
}
```

### Real-Time Monitoring

```cpp
// Periodic statistics reporting
auto stats = engine.getStatistics();
std::cout << "Frames/sec: " << stats.framesSent / duration_seconds << std::endl;
std::cout << "Bandwidth: " << (stats.bytesReceived * 8 / duration_seconds) / 1000000.0 << " Mbps" << std::endl;
```

### Error Handling

```cpp
if (!sender.initialize("192.168.1.100", 5000, 5001)) {
    std::cerr << "Failed to initialize sender" << std::endl;
    // Implement fallback or retry logic
    return -1;
}

if (!sender.configureVideo(1920, 1080, 30, 5000000)) {
    std::cerr << "Failed to configure video" << std::endl;
    return -1;
}

for (int i = 0; i < frames.size(); ++i) {
    if (!sender.sendVideoFrame(frames[i].data(), frames[i].size())) {
        std::cerr << "Failed to send frame " << i << std::endl;
        // Handle send error
    }
}
```

---

## Support & Contributions

For issues, feature requests, or contributions:
- Check existing documentation
- Review example code
- Test with different configurations
- Submit detailed bug reports with reproduction steps

---

## License

This multimedia streaming system is provided as-is for educational and commercial use.

**Last Updated:** December 27, 2025
