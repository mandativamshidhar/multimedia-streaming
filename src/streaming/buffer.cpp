#include "streaming/buffer.h"

namespace multimedia {
namespace streaming {

FrameBuffer::FrameBuffer(size_t maxSize)
    : maxSize_(maxSize)
{
}

bool FrameBuffer::push(std::shared_ptr<codec::Frame> frame)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (buffer_.size() >= maxSize_) {
        return false;
    }
    
    buffer_.push(frame);
    cv_.notify_one();
    
    return true;
}

std::shared_ptr<codec::Frame> FrameBuffer::pop()
{
    std::unique_lock<std::mutex> lock(mutex_);
    
    cv_.wait(lock, [this] { return !buffer_.empty(); });
    
    auto frame = buffer_.front();
    buffer_.pop();
    
    return frame;
}

bool FrameBuffer::tryPop(std::shared_ptr<codec::Frame>& frame)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (buffer_.empty()) {
        return false;
    }
    
    frame = buffer_.front();
    buffer_.pop();
    
    return true;
}

size_t FrameBuffer::size() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.size();
}

bool FrameBuffer::isEmpty() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.empty();
}

bool FrameBuffer::isFull() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.size() >= maxSize_;
}

void FrameBuffer::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    while (!buffer_.empty()) {
        buffer_.pop();
    }
}

} // namespace streaming
} // namespace multimedia
