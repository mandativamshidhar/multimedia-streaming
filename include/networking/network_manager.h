#ifndef NETWORKING_NETWORK_MANAGER_H
#define NETWORKING_NETWORK_MANAGER_H

#include "socket.h"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <string>

namespace multimedia {
namespace networking {

struct NetworkPacket {
    std::vector<uint8_t> data;
    std::string sourceAddress;
    int sourcePort;
    uint64_t timestamp;
};

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();

    // Initialize server or client
    bool initializeServer(int tcpPort, int udpPort);
    bool initializeClient(const std::string& serverAddress, int tcpPort, int udpPort);

    // Send data
    bool sendViaTCP(const std::vector<uint8_t>& data);
    bool sendViaUDP(const std::vector<uint8_t>& data);

    // Receive data (non-blocking)
    bool hasPackets() const;
    NetworkPacket getNextPacket();

    // Status
    bool isInitialized() const { return initialized_; }
    bool isConnected() const;

    // Configuration
    void setReceiveBufferSize(size_t size) { receiveBufferSize_ = size; }
    void setMaxPackets(size_t max) { maxPackets_ = max; }

private:
    std::shared_ptr<Socket> tcpSocket_;
    std::shared_ptr<Socket> udpSocket_;
    
    std::queue<NetworkPacket> packetQueue_;
    mutable std::mutex queueMutex_;
    std::condition_variable queueCV_;
    
    std::thread receiveThread_;
    std::atomic<bool> running_;
    bool initialized_;
    
    size_t receiveBufferSize_;
    size_t maxPackets_;

    // For TCP server: accepted client sockets
    std::vector<std::shared_ptr<Socket>> clientSockets_;
    mutable std::mutex clientMutex_;

    // Mode and server address for UDP client
    bool isServer_ = false;
    std::string serverAddress_;
    int serverUdpPort_ = 0;

    void receiveThreadFunction();
};

} // namespace networking
} // namespace multimedia

#endif // NETWORKING_NETWORK_MANAGER_H
