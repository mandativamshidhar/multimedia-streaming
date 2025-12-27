# Real-Time Multimedia Streaming System

A professional C++ multimedia streaming module for Android/Linux/Windows with H.264/AAC codec integration, TCP/UDP networking, and optimized buffering for low-latency media delivery.

## Project Overview

- **Language**: C++ (C++17 standard)
- **Build System**: CMake 3.10+
- **Platform Support**: Windows, Linux, macOS
- **Codecs**: H.264 (video), AAC (audio)
- **Networking**: TCP/UDP dual-stack
- **Features**: Multithreading, frame buffering, low-latency streaming

## Project Structure

```
.
├── CMakeLists.txt
├── src/
│   ├── main.cpp                          # Main application demo
│   ├── networking/
│   │   ├── socket.cpp                   # TCP/UDP socket implementation
│   │   └── network_manager.cpp          # Network I/O management
│   ├── codec/
│   │   ├── frame.cpp                    # Frame data structure
│   │   ├── h264_encoder.cpp             # H.264 video encoding
│   │   └── aac_encoder.cpp              # AAC audio encoding
│   └── streaming/
│       ├── buffer.cpp                   # Frame buffering with threading
│       └── engine.cpp                   # Core streaming engine
├── include/
│   ├── networking/
│   │   ├── socket.h
│   │   └── network_manager.h
│   ├── codec/
│   │   ├── frame.h
│   │   ├── h264_encoder.h
│   │   └── aac_encoder.h
│   └── streaming/
│       ├── buffer.h
│       └── engine.h
└── build/                                # Build directory (generated)
```

## Key Features

### 1. Networking Module
- **Socket Class**: Cross-platform TCP/UDP socket implementation
  - Blocking and non-blocking modes
  - Buffer size configuration
  - Address reuse for quick reconnection
  
- **NetworkManager**: Manages concurrent TCP/UDP communication
  - Packet queue with thread-safe access
  - Background receive thread for low-latency packet capture
  - Configurable buffer sizes and queue capacity

### 2. Codec Module
- **Frame Class**: Generic multimedia frame container
  - Support for H.264 video and AAC audio
  - Frame type classification (I-frame, P-frame, B-frame)
  - Timestamp and sequence numbering
  
- **H264Encoder**: Video encoding interface
  - Configurable resolution (width × height)
  - Frame rate and bitrate control
  - Automatic keyframe insertion
  
- **AACEncoder**: Audio encoding interface
  - Configurable sample rate and channels
  - Bitrate selection for quality/bandwidth trade-off

### 3. Streaming Engine
- **FrameBuffer**: Thread-safe circular frame buffer
  - Configurable max size (default 100 frames)
  - Blocking and non-blocking pop operations
  - Automatic overflow handling
  
- **StreamingEngine**: Orchestrates the complete streaming pipeline
  - SENDER/RECEIVER mode support
  - Concurrent video and audio handling
  - Real-time statistics (frames encoded/sent/received, bytes received)
  - Low-latency receive with timeout support

### 4. Multithreading & Buffering
- **Background Receive Thread**: Continuously collects network packets
- **Frame Buffering**: Decouples network I/O from processing
- **Lock-Free Statistics**: Atomic counters for performance monitoring
- **Configurable Timeouts**: Fine-grained control over blocking operations

## Building the Project

### Prerequisites

- CMake 3.10 or later
- C++17 compatible compiler (MSVC, GCC, or Clang)
- Windows SDK (for Windows) or system libraries (Linux/macOS)

### Build Steps (Windows PowerShell)

```powershell
# Create and enter build directory
mkdir build -ErrorAction SilentlyContinue
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build . --config Release

# Run the application
.\bin\Release\streaming_app.exe
```

### Build Steps (Linux/macOS)

```bash
# Create and enter build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build . -- -j$(nproc)

# Run the application
./bin/streaming_app
```

## Usage Example

```cpp
#include "streaming/engine.h"

int main() {
    // Create streaming engine in SENDER mode
    multimedia::streaming::StreamingEngine engine(
        multimedia::streaming::StreamingMode::SENDER
    );
    
    // Initialize networking
    engine.initialize("192.168.1.100", 5000, 5001);
    
    // Configure video: 1920x1080 @ 30 FPS, 5 Mbps
    engine.configureVideo(1920, 1080, 30, 5000000);
    
    // Configure audio: 44.1kHz stereo, 128 kbps
    engine.configureAudio(44100, 2, 128000);
    
    // Send video frame (RGB data)
    std::vector<uint8_t> videoFrame(1920 * 1080 * 3);
    engine.sendVideoFrame(videoFrame.data(), videoFrame.size());
    
    // Send audio frame (PCM data)
    std::vector<int16_t> audioFrame(44100);
    engine.sendAudioFrame(audioFrame.data(), audioFrame.size());
    
    // Receive frames
    auto receivedVideo = engine.receiveVideoFrame(100); // 100ms timeout
    auto receivedAudio = engine.receiveAudioFrame(100);
    
    // Get statistics
    auto stats = engine.getStatistics();
    std::cout << "Frames sent: " << stats.framesSent << std::endl;
    
    return 0;
}
```

## Architecture

### Data Flow

```
[Video Capture] → [H.264 Encoder] → [FrameBuffer] → [NetworkManager] → [TCP/UDP Socket]
[Audio Capture] → [AAC Encoder]   ↓                ↓
                                  [Receive Thread] [Packet Queue]
```

### Threading Model

- **Main Thread**: Application logic, encoding, statistics
- **Receive Thread**: Network packet collection (NetworkManager)
- **Buffer Access**: Thread-safe queue with condition variables

## Performance Considerations

1. **Low Latency**: Background receive thread prevents blocking on I/O
2. **Frame Buffering**: Circular buffer (max 100 frames) handles jitter
3. **Configurable Bitrates**: Trade-off between quality and bandwidth
4. **Non-blocking Operations**: Timeout-based pop for responsive applications
5. **Memory Efficiency**: Reusable frame objects, configurable buffer sizes

## Configuration Parameters

### Video Configuration
```cpp
engine.configureVideo(
    1920,        // Width in pixels
    1080,        // Height in pixels
    30,          // Frames per second
    5000000      // Bitrate in bps
);
```

### Audio Configuration
```cpp
engine.configureAudio(
    44100,       // Sample rate (Hz)
    2,           // Channels (1=mono, 2=stereo)
    128000       // Bitrate in bps
);
```

## Extension Points

The system is designed for easy extension:

1. **Replace Codec Implementation**: Implement actual H.264/AAC encoding using x264/libfdk-aac
2. **Add Protocol Support**: Extend NetworkManager for RTMP, RTP, HLS
3. **Implement Decoding**: Create H264Decoder and AACDecoder classes
4. **Add Streaming Protocols**: Implement RTP packetization and RTCP feedback
5. **Platform-Specific Optimization**: Add SIMD optimization for encoding/decoding

## Future Enhancements

- [ ] Integration with x264/libx265 for actual H.264/HEVC encoding
- [ ] Integration with libfdk-aac for real AAC encoding
- [ ] RTP packetization and RTCP feedback
- [ ] Adaptive bitrate streaming (ABR)
- [ ] Frame loss recovery with FEC
- [ ] DASH/HLS support
- [ ] GPU acceleration support
- [ ] Android NDK build integration
- [ ] Performance profiling tools

## Troubleshooting

### Build Issues

- **"Could not find compiler"**: Install Visual Studio Build Tools or GCC
- **"winsock2.h not found"**: Install Windows SDK development headers
- **CMake version**: Ensure CMake 3.10+ is installed

### Runtime Issues

- **"Failed to connect"**: Ensure server is listening on specified port
- **"Socket bind failed"**: Port may be in use; try a different port
- **"Encoder not configured"**: Call `configureVideo`/`configureAudio` before sending frames

## References

- [CMake Documentation](https://cmake.org/documentation/)
- [C++17 Standard](https://en.cppreference.com/)
- H.264/AVC Codec Standard (ITU-T H.264)
- AAC Audio Codec (ISO/IEC 13818-7)

## License

This project is provided as-is for educational and commercial use.

## Author

Multimedia Streaming System Development Team
