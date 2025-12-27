cmake_minimum_required(VERSION 3.10)
project(MultimediaStreamingSystem VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files - Networking module
set(NETWORKING_SOURCES
    src/networking/socket.cpp
    src/networking/network_manager.cpp
)

# Source files - Codec module
set(CODEC_SOURCES
    src/codec/frame.cpp
    src/codec/h264_encoder.cpp
    src/codec/aac_encoder.cpp
)

# Source files - Streaming module
set(STREAMING_SOURCES
    src/streaming/buffer.cpp
    src/streaming/engine.cpp
)

# Main application
set(APP_SOURCES
    src/main.cpp
)

# Combine all sources
set(ALL_SOURCES
    ${NETWORKING_SOURCES}
    ${CODEC_SOURCES}
    ${STREAMING_SOURCES}
    ${APP_SOURCES}
)

# Create executable
add_executable(streaming_app ${ALL_SOURCES})

# Link with system libraries (Windows socket library)
if(WIN32)
    target_link_libraries(streaming_app PRIVATE ws2_32)
endif()

# Optional: Add compiler flags for warnings
if(MSVC)
    target_compile_options(streaming_app PRIVATE /W4 /std:c++17)
else()
    target_compile_options(streaming_app PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Enable testing
enable_testing()
