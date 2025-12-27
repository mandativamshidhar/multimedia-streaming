#include "networking/network_manager.h"
#include <iostream>

namespace multimedia {
namespace networking {

NetworkManager::NetworkManager()
    : running_(false), initialized_(false), receiveBufferSize_(65536), maxPackets_(100)
{
}

NetworkManager::~NetworkManager()
{
    running_ = false;
    if (receiveThread_.joinable()) {
        receiveThread_.join();
    }
}

bool NetworkManager::initializeServer(int tcpPort, int udpPort)
{
    // Create TCP socket
    tcpSocket_ = std::make_shared<Socket>(SocketType::TCP, SocketMode::SERVER);
    if (!tcpSocket_->bind("0.0.0.0", tcpPort) || !tcpSocket_->listen()) {
        std::cerr << "Failed to initialize TCP server" << std::endl;
        return false;
    }
    
    // Create UDP socket
    udpSocket_ = std::make_shared<Socket>(SocketType::UDP, SocketMode::SERVER);
    if (!udpSocket_->bind("0.0.0.0", udpPort)) {
        std::cerr << "Failed to initialize UDP server" << std::endl;
        return false;
    }
    
    udpSocket_->setNonBlocking(true);
    // Make TCP listening socket non-blocking so accept() is non-blocking
    tcpSocket_->setNonBlocking(true);
    
    initialized_ = true;
    isServer_ = true;
    running_ = true;
    receiveThread_ = std::thread(&NetworkManager::receiveThreadFunction, this);
    
    std::cout << "NetworkManager initialized as server (TCP:" << tcpPort 
              << ", UDP:" << udpPort << ")" << std::endl;
    
    return true;
}

bool NetworkManager::initializeClient(const std::string& serverAddress, int tcpPort, int udpPort)
{
    // Create TCP socket
    tcpSocket_ = std::make_shared<Socket>(SocketType::TCP, SocketMode::CLIENT);
    if (!tcpSocket_->connect(serverAddress, tcpPort)) {
        std::cerr << "Failed to connect TCP to server" << std::endl;
        return false;
    }
    
    // Create UDP socket
    udpSocket_ = std::make_shared<Socket>(SocketType::UDP, SocketMode::CLIENT);
    // store server address/port for UDP sends
    serverAddress_ = serverAddress;
    serverUdpPort_ = udpPort;
    
    initialized_ = true;
    isServer_ = false;
    running_ = true;
    receiveThread_ = std::thread(&NetworkManager::receiveThreadFunction, this);
    
    std::cout << "NetworkManager initialized as client (server: " << serverAddress << ")" << std::endl;
    
    return true;
}

bool NetworkManager::sendViaTCP(const std::vector<uint8_t>& data)
{
    if (!tcpSocket_ || !tcpSocket_->isConnected()) {
        return false;
    }
    return tcpSocket_->send(data);
}

bool NetworkManager::sendViaUDP(const std::vector<uint8_t>& data)
{
    if (!udpSocket_) return false;
    if (!isServer_ && !serverAddress_.empty() && serverUdpPort_ > 0) {
        return udpSocket_->sendTo(serverAddress_, serverUdpPort_, data);
    }
    // Server mode: no specific peer to send to in this simple implementation
    return false;
}

bool NetworkManager::hasPackets() const
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    return !packetQueue_.empty();
}

NetworkPacket NetworkManager::getNextPacket()
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    if (packetQueue_.empty()) {
        return NetworkPacket{};
    }
    
    auto packet = packetQueue_.front();
    packetQueue_.pop();
    return packet;
}

bool NetworkManager::isConnected() const
{
    return tcpSocket_ && tcpSocket_->isConnected();
}

void NetworkManager::receiveThreadFunction()
{
    while (running_) {
        // First, accept any new TCP clients (server mode)
        if (isServer_ && tcpSocket_) {
            auto client = tcpSocket_->accept();
            if (client) {
                client->setNonBlocking(true);
                std::lock_guard<std::mutex> lock(clientMutex_);
                clientSockets_.push_back(client);
            }
        }

        // Read from TCP client sockets
        if (!clientSockets_.empty()) {
            std::lock_guard<std::mutex> lock(clientMutex_);
            for (auto it = clientSockets_.begin(); it != clientSockets_.end();) {
                auto sock = *it;
                if (!sock) { it = clientSockets_.erase(it); continue; }
                auto data = sock->recv(receiveBufferSize_);
                if (!data.empty()) {
                    std::lock_guard<std::mutex> qlock(queueMutex_);
                    if (packetQueue_.size() < maxPackets_) {
                        packetQueue_.push({data, "", 0, 0});
                    }
                }
                if (!sock->isConnected()) {
                    it = clientSockets_.erase(it);
                } else {
                    ++it;
                }
            }
        }

        if (udpSocket_) {
            std::string address;
            int port;
            auto data = udpSocket_->recvFrom(address, port, receiveBufferSize_);
            
            if (!data.empty()) {
                std::lock_guard<std::mutex> lock(queueMutex_);
                if (packetQueue_.size() < maxPackets_) {
                    packetQueue_.push({data, address, port, 0});
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

} // namespace networking
} // namespace multimedia
