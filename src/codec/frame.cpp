#include "codec/frame.h"

namespace multimedia {
namespace codec {

Frame::Frame(CodecType codecType, FrameType frameType)
    : codecType_(codecType), frameType_(frameType), timestamp_(0), sequenceNumber_(0)
{
}

void Frame::appendData(const uint8_t* data, size_t size)
{
    if (data && size > 0) {
        data_.insert(data_.end(), data, data + size);
    }
}

void Frame::setData(const std::vector<uint8_t>& data)
{
    data_ = data;
}

} // namespace codec
} // namespace multimedia
