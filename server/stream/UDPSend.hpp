/*
 *  UDPSend.h
 *  MySender
 *
 *  Created by Helmut Hlavacs on 10.12.10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include <cstdint>
#include <thread>
extern "C" {
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
}
#include "NetworkDefines.hpp"

class UDPSend {

public:
	int sock;
	struct sockaddr_in addr;
	uint8_t packetnum;
    FrameStats stats{};

	std::thread stat_thread;

	UDPSend();
	~UDPSend(){ if(sock) close(sock); };
	void init(const char *address, int port, int recv_port);
	int send(const uint8_t *buffer, size_t len, int width, int height);
	ssize_t recv(uint8_t* buffer, size_t len);
	void send_sound(Sound sound);
	void send_music(Music music);
	void closeSock();
};



