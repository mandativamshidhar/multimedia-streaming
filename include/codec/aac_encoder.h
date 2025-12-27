#ifndef CODEC_AAC_ENCODER_H
#define CODEC_AAC_ENCODER_H

#include "frame.h"
#include <memory>

namespace multimedia {
namespace codec {

class AACEncoder {
public:
    AACEncoder();
    ~AACEncoder();

    // Encoder configuration
    bool configure(int sampleRate, int channels, int bitrate);
    
    // Encoding
    std::shared_ptr<Frame> encodeFrame(const int16_t* audioData, 
                                       size_t sampleCount);

    // Status
    bool isConfigured() const { return configured_; }
    int getSampleRate() const { return sampleRate_; }
    int getChannels() const { return channels_; }
    int getBitrate() const { return bitrate_; }

private:
    int sampleRate_;
    int channels_;
    int bitrate_;
    bool configured_;
    uint32_t frameCount_;
    
    // Placeholder for actual AAC encoding library (e.g., libfdk-aac)
};

} // namespace codec
} // namespace multimedia

#endif // CODEC_AAC_ENCODER_H
