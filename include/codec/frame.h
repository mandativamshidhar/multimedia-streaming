#ifndef CODEC_FRAME_H
#define CODEC_FRAME_H

#include <vector>
#include <cstdint>
#include <memory>

namespace multimedia {
namespace codec {

enum class CodecType {
    H264_VIDEO,
    AAC_AUDIO
};

enum class FrameType {
    I_FRAME,    // Intra frame (keyframe)
    P_FRAME,    // Predictive frame
    B_FRAME     // Bidirectional frame
};

class Frame {
public:
    Frame(CodecType codecType, FrameType frameType = FrameType::I_FRAME);
    ~Frame() = default;

    // Data management
    void appendData(const uint8_t* data, size_t size);
    void setData(const std::vector<uint8_t>& data);
    const std::vector<uint8_t>& getData() const { return data_; }

    // Metadata
    void setTimestamp(uint64_t ts) { timestamp_ = ts; }
    uint64_t getTimestamp() const { return timestamp_; }
    
    void setSequenceNumber(uint32_t seq) { sequenceNumber_ = seq; }
    uint32_t getSequenceNumber() const { return sequenceNumber_; }
    
    CodecType getCodecType() const { return codecType_; }
    FrameType getFrameType() const { return frameType_; }
    
    size_t getSize() const { return data_.size(); }
    
    bool isKeyFrame() const { return frameType_ == FrameType::I_FRAME; }

private:
    std::vector<uint8_t> data_;
    CodecType codecType_;
    FrameType frameType_;
    uint64_t timestamp_;
    uint32_t sequenceNumber_;
};

} // namespace codec
} // namespace multimedia

#endif // CODEC_FRAME_H
