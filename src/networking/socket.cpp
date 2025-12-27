#include "networking/socket.h"
#include <iostream>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
    
    // Winsock initialization
    class WinsockInitializer {
    public:
        WinsockInitializer() {
            WSADATA wsaData;
            int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (result != 0) {
                std::cerr << "WSAStartup failed: " << result << std::endl;
            }
        }
        ~WinsockInitializer() {
            WSACleanup();
        }
    };
    
    static WinsockInitializer winsockInit;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

namespace multimedia {
namespace networking {

Socket::Socket(SocketType type, SocketMode mode)
    : fd_(INVALID_SOCKET), type_(type), mode_(mode), connected_(false)
{
    // Create socket
    int socketType = (type == SocketType::TCP) ? SOCK_STREAM : SOCK_DGRAM;
    fd_ = socket(AF_INET, socketType, 0);
    
    if (fd_ == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
    }
}

Socket::~Socket()
{
    cleanup();
}

bool Socket::bind(const std::string& address, int port)
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    if (address.empty() || address == "0.0.0.0") {
        addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        inet_pton(AF_INET, address.c_str(), &addr.sin_addr);
    }
    
    if (::bind(fd_, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed on port " << port << std::endl;
        return false;
    }
    
    return true;
}

bool Socket::connect(const std::string& address, int port)
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, address.c_str(), &addr.sin_addr);
    
    if (::connect(fd_, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "Connect failed to " << address << ":" << port << std::endl;
        return false;
    }
    
    connected_ = true;
    return true;
}

bool Socket::listen(int backlog)
{
    if (::listen(fd_, backlog) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        return false;
    }
    return true;
}

std::shared_ptr<Socket> Socket::accept()
{
    sockaddr_in clientAddr{};
    socklen_t addrLen = sizeof(clientAddr);
    
    int clientFd = ::accept(fd_, (sockaddr*)&clientAddr, &addrLen);
    if (clientFd == INVALID_SOCKET) {
#ifdef _WIN32
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) return nullptr;
        std::cerr << "Accept failed: " << err << std::endl;
#else
        if (errno == EWOULDBLOCK || errno == EAGAIN) return nullptr;
        std::cerr << "Accept failed: " << strerror(errno) << std::endl;
#endif
        return nullptr;
    }
    
    auto clientSocket = std::make_shared<Socket>(type_, SocketMode::CLIENT);
    clientSocket->fd_ = clientFd;
    clientSocket->connected_ = true;
    
    return clientSocket;
}

bool Socket::send(const std::vector<uint8_t>& data)
{
    if (!connected_ && type_ == SocketType::TCP) {
        std::cerr << "Socket not connected" << std::endl;
        return false;
    }
    
    size_t totalSent = 0;
    while (totalSent < data.size()) {
        int sent = ::send(fd_, (const char*)data.data() + totalSent, 
                         data.size() - totalSent, 0);
        if (sent == SOCKET_ERROR) {
            std::cerr << "Send failed" << std::endl;
            return false;
        }
        totalSent += sent;
    }
    
    return true;
}

bool Socket::sendTo(const std::string& address, int port, const std::vector<uint8_t>& data)
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, address.c_str(), &addr.sin_addr);
    
    int sent = ::sendto(fd_, (const char*)data.data(), data.size(), 0, 
                       (sockaddr*)&addr, sizeof(addr));
    
    if (sent == SOCKET_ERROR) {
        std::cerr << "SendTo failed" << std::endl;
        return false;
    }
    
    return true;
}

std::vector<uint8_t> Socket::recv(size_t maxSize)
{
    std::vector<uint8_t> buffer(maxSize);
    int received = ::recv(fd_, (char*)buffer.data(), maxSize, 0);
    
    if (received <= 0) {
        if (received == 0) {
            connected_ = false;
        }
        return std::vector<uint8_t>();
    }
    
    buffer.resize(received);
    return buffer;
}

std::vector<uint8_t> Socket::recvFrom(std::string& address, int& port, size_t maxSize)
{
    std::vector<uint8_t> buffer(maxSize);
    sockaddr_in addr{};
    socklen_t addrLen = sizeof(addr);
    
    int received = ::recvfrom(fd_, (char*)buffer.data(), maxSize, 0, 
                             (sockaddr*)&addr, &addrLen);
    
    if (received <= 0) {
        return std::vector<uint8_t>();
    }
    
    buffer.resize(received);
    
    char addrStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, addrStr, INET_ADDRSTRLEN);
    address = addrStr;
    port = ntohs(addr.sin_port);
    
    return buffer;
}

bool Socket::setNonBlocking(bool nonBlocking)
{
#ifdef _WIN32
    u_long mode = nonBlocking ? 1 : 0;
    return ioctlsocket(fd_, FIONBIO, &mode) != SOCKET_ERROR;
#else
    int flags = fcntl(fd_, F_GETFL, 0);
    if (flags == -1) return false;
    return fcntl(fd_, F_SETFL, nonBlocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK)) != -1;
#endif
}

bool Socket::setReuseAddress(bool reuse)
{
    int opt = reuse ? 1 : 0;
    return setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) != SOCKET_ERROR;
}

bool Socket::setReceiveBufferSize(int size)
{
    return setsockopt(fd_, SOL_SOCKET, SO_RCVBUF, (const char*)&size, sizeof(size)) != SOCKET_ERROR;
}

bool Socket::setSendBufferSize(int size)
{
    return setsockopt(fd_, SOL_SOCKET, SO_SNDBUF, (const char*)&size, sizeof(size)) != SOCKET_ERROR;
}

void Socket::cleanup()
{
    if (fd_ != INVALID_SOCKET) {
        closesocket(fd_);
        fd_ = INVALID_SOCKET;
    }
    connected_ = false;
}

} // namespace networking
} // namespace multimedia
