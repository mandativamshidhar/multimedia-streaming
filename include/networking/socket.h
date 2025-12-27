#ifndef NETWORKING_SOCKET_H
#define NETWORKING_SOCKET_H

#include <string>
#include <vector>
#include <memory>

namespace multimedia {
namespace networking {

enum class SocketType {
    TCP,
    UDP
};

enum class SocketMode {
    CLIENT,
    SERVER
};

class Socket {
public:
    Socket(SocketType type, SocketMode mode);
    ~Socket();

    // Connection management
    bool bind(const std::string& address, int port);
    bool connect(const std::string& address, int port);
    bool listen(int backlog = 1);
    std::shared_ptr<Socket> accept();

    // Data transmission
    bool send(const std::vector<uint8_t>& data);
    bool sendTo(const std::string& address, int port, const std::vector<uint8_t>& data);
    std::vector<uint8_t> recv(size_t maxSize = 65536);
    std::vector<uint8_t> recvFrom(std::string& address, int& port, size_t maxSize = 65536);

    // Configuration
    bool setNonBlocking(bool nonBlocking);
    bool setReuseAddress(bool reuse);
    bool setReceiveBufferSize(int size);
    bool setSendBufferSize(int size);

    // Status
    bool isConnected() const { return connected_; }
    int getFileDescriptor() const { return fd_; }
    SocketType getType() const { return type_; }

private:
    int fd_;
    SocketType type_;
    SocketMode mode_;
    bool connected_;

    void cleanup();
};

} // namespace networking
} // namespace multimedia

#endif // NETWORKING_SOCKET_H
