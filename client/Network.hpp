#pragma once
#include <cstdint>
#include <chrono>
#include <mutex>
#include "NetworkDefines.hpp"

// Include udp libs
extern "C" {
    #include <netinet/in.h>
    #include <sys/socket.h>
}

class Client;

class Network {
private:
    Client* m_client;

    int sock_fd;
    sockaddr_in server_addr;

    std::mutex heartbeat_lock{};
    std::chrono::time_point<std::chrono::steady_clock> last_heartbeat;

    std::chrono::time_point<std::chrono::steady_clock> last_stats;
    bool m_connected;

    FrameStats stats{};

    uint8_t recv_buffer[MAX_DATA];
    ssize_t nbytes_unread = 0;

    void send(const void *msg, size_t len);

public:
    Network(Client* client, uint16_t port = 8080);
    ~Network();

    bool receive(
        ReceivedFrame* frame
    );
	
    //bool send(
    //    const uint8_t *buffer,
    //    size_t len
    //);
    
    void check_heartbeat();
    void send_keydown(int key_code);
    void send_keyup(int key_code);
};
