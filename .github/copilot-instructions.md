# Real-Time Multimedia Streaming System

This is a professional C++ multimedia streaming module for real-time video/audio transmission over TCP/UDP networks with H.264/AAC codec support.

## Project Overview

- **Language**: C++ (C++17 standard)
- **Build System**: CMake 3.10+
- **Platform Support**: Windows, Linux, macOS
- **Codecs**: H.264 (video), AAC (audio)
- **Networking**: TCP/UDP dual-stack with async I/O
- **Features**: Multithreading, frame buffering, low-latency optimization

## Project Structure

```
.
├── CMakeLists.txt
├── src/
│   ├── main.cpp                    # Demo application
│   ├── networking/                 # TCP/UDP networking
│   │   ├── socket.cpp
│   │   └── network_manager.cpp
│   ├── codec/                      # H.264/AAC codec
│   │   ├── frame.cpp
│   │   ├── h264_encoder.cpp
│   │   └── aac_encoder.cpp
│   └── streaming/                  # Core streaming engine
│       ├── buffer.cpp
│       └── engine.cpp
├── include/                        # Header files (mirrors src/)
├── build/                          # Build directory (generated)
└── README.md                       # Comprehensive documentation
```

## Core Components

### 1. Networking Module (`src/networking/`)
- **Socket**: Cross-platform TCP/UDP socket wrapper
- **NetworkManager**: Thread-safe network I/O with packet queue

### 2. Codec Module (`src/codec/`)
- **Frame**: Universal multimedia frame container
- **H264Encoder**: Video encoder interface
- **AACEncoder**: Audio encoder interface

### 3. Streaming Engine (`src/streaming/`)
- **FrameBuffer**: Thread-safe circular buffer for frames
- **StreamingEngine**: Main streaming orchestrator with SENDER/RECEIVER modes

## Building the Project

### Prerequisites

- CMake 3.10 or later
- C++17 compatible compiler
- Windows SDK (Windows) or system libraries (Linux/macOS)

### Build Steps (Windows PowerShell)

```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
.\bin\Release\streaming_app.exe
```

### Build Steps (Linux/macOS)

```bash
mkdir -p build
cd build
cmake ..
cmake --build . -- -j$(nproc)
./bin/streaming_app
```

## Key Features

### 1. Network Stack
- TCP for reliable control/data transmission
- UDP for low-latency media streaming
- Non-blocking socket operations
- Configurable buffer sizes
- Cross-platform socket API

### 2. Codec System
- H.264 video frame encoding
- AAC audio frame encoding
- Frame type classification (I/P/B frames)
- Sequence numbering and timestamps
- Placeholder for actual codec integration (x264, libfdk-aac)

### 3. Streaming Pipeline
- Parallel video/audio processing
- Frame buffering (default 100 frames)
- Background receive thread
- Automatic packet queuing
- Real-time statistics tracking

### 4. Performance Optimizations
- Non-blocking I/O for low latency
- Circular frame buffers to prevent blocking
- Atomic counters for statistics
- Thread-safe queue with condition variables
- Configurable timeouts for responsive processing

## Architecture

```
Sender Side:
[Video/Audio Capture] → [Encoder] → [Buffer] → [Network] → [Socket]

Receiver Side:
[Socket] → [Network] → [Buffer] → [Application]
```

## Usage Example

```cpp
#include "streaming/engine.h"

multimedia::streaming::StreamingEngine engine(
    multimedia::streaming::StreamingMode::SENDER
);

engine.initialize("192.168.1.100", 5000, 5001);
engine.configureVideo(1920, 1080, 30, 5000000);   // 5 Mbps
engine.configureAudio(44100, 2, 128000);          // 128 kbps

// Send frames
engine.sendVideoFrame(rgbData, dataSize);
engine.sendAudioFrame(audioData, sampleCount);

// Statistics
auto stats = engine.getStatistics();
```

## Configuration

### Video Settings
- Resolution: Up to 4K (3840×2160)
- Frame rate: 1-60 FPS
- Bitrate: Configurable (100kbps - 50Mbps)

### Audio Settings
- Sample rates: 8kHz, 16kHz, 22.05kHz, 44.1kHz, 48kHz
- Channels: 1 (mono) or 2 (stereo)
- Bitrate: 32kbps - 320kbps

### Network Settings
- TCP port: Configurable (default 5000)
- UDP port: Configurable (default 5001)
- Buffer size: Configurable (default 65536 bytes)
- Max queued packets: Configurable (default 100)

## Extension Points

1. **Actual Codec Implementation**: Replace placeholder encoders with x264/libfdk-aac
2. **Protocol Enhancements**: Add RTP, RTMP, HLS support
3. **Decoder Implementation**: Create H264Decoder/AACDecoder classes
4. **GPU Acceleration**: Add CUDA/OpenGL optimization
5. **Platform-Specific**: Android NDK integration, iOS support

## Compilation

VS Code integration with tasks.json provides:
- CMake: Configure
- CMake: Build (default with Ctrl+Shift+B)
- CMake: Build & Run

Or build manually from `build/` directory:
```powershell
cmake --build . --config Release
```

## Testing

Demo application in `src/main.cpp` includes:
- Video frame simulation (1920×1080 RGB)
- Audio frame simulation (44.1kHz PCM)
- Frame encoding and transmission
- Real-time statistics output

Run the demo to verify all components work together.

## Next Steps

1. Integrate actual H.264/AAC libraries (x264, libfdk-aac)
2. Implement frame decoders
3. Add RTP packetization
4. Implement RTCP feedback
5. Add adaptive bitrate control
6. Profile and optimize performance
7. Create unit tests for each module
8. Document codec-specific optimizations

## Troubleshooting

- **Build fails**: Ensure CMake 3.10+ and C++17 compiler installed
- **Linker errors on Linux**: Install `libasan` or disable sanitizers
- **Network errors**: Check firewall, ports must be available
- **Frame drop**: Increase buffer size with `setMaxSize()`

## References

- CMake: https://cmake.org/documentation/
- H.264 Standard: ITU-T H.264/AVC
- AAC Standard: ISO/IEC 13818-7
