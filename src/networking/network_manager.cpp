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
    
    initialized_ = true;
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
    
    initialized_ = true;
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
    if (!udpSocket_) {
        return false;
    }
    return true; // Would need peer address for client mode
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
