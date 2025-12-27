#include "codec/h264_encoder.h"
#include <iostream>

namespace multimedia {
namespace codec {

H264Encoder::H264Encoder()
    : width_(0), height_(0), fps_(0), bitrate_(0), configured_(false), frameCount_(0)
{
}

H264Encoder::~H264Encoder()
{
}

bool H264Encoder::configure(int width, int height, int fps, int bitrate)
{
    if (width <= 0 || height <= 0 || fps <= 0 || bitrate <= 0) {
        std::cerr << "Invalid H.264 encoder parameters" << std::endl;
        return false;
    }
    
    width_ = width;
    height_ = height;
    fps_ = fps;
    bitrate_ = bitrate;
    configured_ = true;
    
    std::cout << "H.264 Encoder configured: " << width << "x" << height 
              << "@" << fps << "fps, " << bitrate << "bps" << std::endl;
    
    return true;
}

std::vector<std::shared_ptr<Frame>> H264Encoder::encodeFrame(const uint8_t* rgbData, 
                                                             size_t dataSize,
                                                             bool forceKeyframe)
{
    std::vector<std::shared_ptr<Frame>> encodedFrames;
    
    if (!configured_) {
        std::cerr << "Encoder not configured" << std::endl;
        return encodedFrames;
    }
    
    // Placeholder: actual encoding would use x264 or similar library
    FrameType ftype = (forceKeyframe || frameCount_ % 30 == 0) ? FrameType::I_FRAME : FrameType::P_FRAME;
    
    auto frame = std::make_shared<Frame>(CodecType::H264_VIDEO, ftype);
    
    // Simulate encoding by using input data
    if (rgbData && dataSize > 0) {
        // In real scenario, would encode RGB to H.264 bitstream
        frame->appendData(rgbData, std::min(dataSize, size_t(4096))); // Simulate encoded data
    }
    
    frame->setTimestamp(frameCount_);
    frame->setSequenceNumber(frameCount_);
    
    encodedFrames.push_back(frame);
    frameCount_++;
    
    return encodedFrames;
}

} // namespace codec
} // namespace multimedia
