#ifndef STREAMING_BUFFER_H
#define STREAMING_BUFFER_H

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "codec/frame.h"

namespace multimedia {
namespace streaming {

class FrameBuffer {
public:
    explicit FrameBuffer(size_t maxSize = 100);
    ~FrameBuffer() = default;

    // Push frame to buffer
    bool push(std::shared_ptr<codec::Frame> frame);
    
    // Pop frame from buffer (blocks if empty)
    std::shared_ptr<codec::Frame> pop();
    
    // Non-blocking pop
    bool tryPop(std::shared_ptr<codec::Frame>& frame);
    
    // Buffer status
    size_t size() const;
    bool isEmpty() const;
    bool isFull() const;
    
    // Clear buffer
    void clear();
    
    // Configuration
    void setMaxSize(size_t maxSize) { maxSize_ = maxSize; }
    size_t getMaxSize() const { return maxSize_; }

private:
    std::queue<std::shared_ptr<codec::Frame>> buffer_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    size_t maxSize_;
};

} // namespace streaming
} // namespace multimedia

#endif // STREAMING_BUFFER_H
