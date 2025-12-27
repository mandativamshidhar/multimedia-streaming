# Distribution & Deployment Guide

## How Others Can Use This Project

### Option 1: Direct Download & Build

**For Developers:**

1. Clone/download the project
```bash
git clone <repository-url>
# or download ZIP and extract
```

2. Follow the build instructions in GETTING_STARTED.md
```powershell
cd Project
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

3. Use the library in their own projects
```cpp
#include "streaming/engine.h"
// Link against the compiled libraries
```

---

### Option 2: Pre-built Binaries

**For End Users:**

Distribute pre-compiled executables:
```
streaming_app.exe
```

They can run directly without compilation:
```powershell
streaming_app.exe
```

---

### Option 3: Library Integration

**For Software Projects:**

1. Copy the project into their codebase:
```
their_project/
├── multimedia_streaming/
│   ├── CMakeLists.txt
│   ├── src/
│   ├── include/
│   └── ...
```

2. Include in their CMakeLists.txt:
```cmake
add_subdirectory(multimedia_streaming)
target_link_libraries(their_app multimedia_streaming)
```

3. Use the API:
```cpp
#include "multimedia_streaming/streaming/engine.h"
```

---

## Packaging for Distribution

### Create a Release Package

```powershell
# 1. Clean build
cd Project
rm -r build
mkdir build
cd build
cmake ..
cmake --build . --config Release

# 2. Copy files for distribution
$release = "MultimediaStreamingSystem_v1.0"
mkdir $release
copy bin\streaming_app.exe $release\
copy ..\README.md $release\
copy ..\USAGE_GUIDE.md $release\
copy ..\GETTING_STARTED.md $release\
copy ..\examples.cpp $release\

# 3. Create ZIP
Compress-Archive -Path $release -DestinationPath $release.zip

# Done! Distribute $release.zip
```

---

## Documentation Provided

### For Users
- **GETTING_STARTED.md** - Quick start guide
- **USAGE_GUIDE.md** - Comprehensive API reference
- **README.md** - Project overview
- **examples.cpp** - Code examples

### For Developers
- **Source code** - Well-commented, modular design
- **CMakeLists.txt** - Build configuration
- **Header files** - Clear API documentation
- **Inline comments** - Explain complex sections

---

## Running the Executable

### From Command Line

**Simple Demo (Receiver + Sender):**
```powershell
streaming_app.exe
```

**With Parameters (Future Enhancement):**
```powershell
streaming_app.exe --mode receiver --port 5000 --video 1920x1080 --audio 44100
```

### From PowerShell Script

```powershell
# Run and log output
& "C:\path\to\streaming_app.exe" | Out-File -FilePath "output.log"

# Run in background
Start-Process "C:\path\to\streaming_app.exe"

# Run with timeout
$process = Start-Process "C:\path\to\streaming_app.exe" -PassThru
Start-Sleep -Seconds 60
$process | Stop-Process
```

### From Batch Script

Create `run.bat`:
```batch
@echo off
cd "C:\Users\manda\Git_projects\Project\build\bin"
streaming_app.exe
pause
```

Then double-click to run.

---

## Integrating Into Own Project

### Step 1: Copy Project Files

```
YourProject/
├── CMakeLists.txt
├── src/
│   └── main.cpp
└── deps/
    └── multimedia_streaming/
        ├── CMakeLists.txt
        ├── src/
        ├── include/
        └── ...
```

### Step 2: Update CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.10)
project(YourProject)

# Add multimedia streaming
add_subdirectory(deps/multimedia_streaming)

# Your executable
add_executable(your_app src/main.cpp)

# Link streaming library
target_link_libraries(your_app PRIVATE streaming_system)
target_include_directories(your_app PRIVATE deps/multimedia_streaming/include)
```

### Step 3: Use in Code

```cpp
#include <iostream>
#include "streaming/engine.h"

int main() {
    multimedia::streaming::StreamingEngine engine(
        multimedia::streaming::StreamingMode::SENDER
    );
    
    if (engine.initialize("192.168.1.100", 5000, 5001)) {
        engine.configureVideo(1920, 1080, 30, 5000000);
        // ... streaming code ...
    }
    
    return 0;
}
```

### Step 4: Build

```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

---

## Docker Container (Optional)

For easy deployment across systems:

Create `Dockerfile`:
```dockerfile
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git

WORKDIR /app
COPY . .

RUN mkdir build && cd build && \
    cmake .. && \
    cmake --build . --config Release

EXPOSE 5000/tcp 5001/udp

CMD ["./build/bin/streaming_app"]
```

Build and run:
```bash
docker build -t multimedia-streaming .
docker run -p 5000:5000 -p 5001:5001 multimedia-streaming
```

---

## System Requirements

### Minimum
- **CPU:** Dual-core 1.5 GHz
- **RAM:** 512 MB
- **Storage:** 50 MB
- **Network:** 1 Mbps

### Recommended
- **CPU:** Quad-core 2.5 GHz
- **RAM:** 2 GB
- **Storage:** 100 MB
- **Network:** 10 Mbps

### For 4K Streaming
- **CPU:** 8-core 3+ GHz
- **RAM:** 8 GB
- **Storage:** 500 MB
- **Network:** 100 Mbps

---

## Compatibility Matrix

| Platform | Architecture | Support |
|----------|-------------|---------|
| Windows  | x86_64      | ✅ Full |
| Windows  | ARM64       | ⚠️ Untested |
| Linux    | x86_64      | ✅ Full |
| Linux    | ARM64       | ✅ Full |
| macOS    | x86_64      | ✅ Full |
| macOS    | ARM64       | ✅ Full |

---

## Troubleshooting Deployment

### Issue: "DLL not found"
**On Windows:** Missing Visual C++ Runtime
**Solution:** 
- Download Visual C++ Redistributable from Microsoft
- Or use static linking in CMake

### Issue: "Cannot bind to port"
**Cause:** Port already in use
**Solution:**
```powershell
# Find what's using the port
netstat -ano | findstr :5000

# Kill the process
taskkill /PID <PID> /F

# Or use different port
# Edit CMakeLists.txt or use environment variables
```

### Issue: "Low performance on remote servers"
**Cause:** Network latency or bandwidth
**Solution:**
- Use UDP instead of TCP where possible
- Reduce resolution/bitrate
- Implement adaptive bitrate control
- Use content delivery network (CDN)

---

## Configuration Management

### Environment Variables

```powershell
# Set port
$env:STREAMING_TCP_PORT = "6000"
$env:STREAMING_UDP_PORT = "6001"

# Run application
.\streaming_app.exe
```

### Configuration File (Future)

```json
{
  "server": {
    "tcp_port": 5000,
    "udp_port": 5001,
    "listen_address": "0.0.0.0"
  },
  "video": {
    "width": 1920,
    "height": 1080,
    "fps": 30,
    "bitrate": 5000000
  },
  "audio": {
    "sample_rate": 44100,
    "channels": 2,
    "bitrate": 128000
  }
}
```

---

## Updates & Maintenance

### Checking for Updates
```bash
git fetch origin
git log --oneline -10  # See latest changes
```

### Applying Updates
```bash
git pull origin main
# Rebuild
cd build
cmake --build . --config Release
```

### Version Tracking
Current version: **1.0.0**
- Check CMakeLists.txt for version number
- Update version when making releases

---

## License & Distribution

Ensure proper licensing when distributing:

**Current License:** Open source (see LICENSE file)

**Allowed Uses:**
- ✅ Educational purposes
- ✅ Commercial applications
- ✅ Modification and redistribution
- ✅ Integration into other projects

**Required Actions:**
- Include original license with distribution
- Provide source code or link to source
- Credit original authors

---

## Getting Help

### Resources
1. **GETTING_STARTED.md** - Basic usage
2. **USAGE_GUIDE.md** - Detailed API
3. **examples.cpp** - Code examples
4. **README.md** - Project overview
5. **Source code comments** - Implementation details

### Debugging
```cpp
// Enable verbose output
#define DEBUG_STREAMING 1

// Or use logging
std::cerr << "Debug message: " << variable << std::endl;
```

### Reporting Issues
When reporting problems, include:
- Error message
- Platform (Windows/Linux/macOS)
- Steps to reproduce
- Expected vs actual behavior

---

## Performance Tips

### For Better Streaming
1. **Reduce resolution** for low bandwidth
2. **Lower frame rate** if CPU-bound
3. **Use UDP** for real-time
4. **Increase buffer** for stability
5. **Monitor statistics** for optimization

### For Better User Experience
1. Implement buffering strategy
2. Add error recovery
3. Provide network status feedback
4. Allow quality adjustment
5. Log performance metrics

---

**Last Updated:** December 27, 2025
