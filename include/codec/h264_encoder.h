#ifndef CODEC_H264_ENCODER_H
#define CODEC_H264_ENCODER_H

#include "frame.h"
#include <memory>

namespace multimedia {
namespace codec {

class H264Encoder {
public:
    H264Encoder();
    ~H264Encoder();

    // Encoder configuration
    bool configure(int width, int height, int fps, int bitrate);
    
    // Encoding
    std::vector<std::shared_ptr<Frame>> encodeFrame(const uint8_t* rgbData, 
                                                     size_t dataSize,
                                                     bool forceKeyframe = false);

    // Status
    bool isConfigured() const { return configured_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    int getFPS() const { return fps_; }
    int getBitrate() const { return bitrate_; }

private:
    int width_;
    int height_;
    int fps_;
    int bitrate_;
    bool configured_;
    uint32_t frameCount_;
    
    // Placeholder for actual H.264 encoding library (e.g., x264)
};

} // namespace codec
} // namespace multimedia

#endif // CODEC_H264_ENCODER_H
