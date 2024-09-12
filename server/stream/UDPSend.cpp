#include "UDPSend.hpp"
#include "NetworkDefines.hpp"
#include <cassert>
#include <ctime>
#include <iostream>

#define min(x,y)  ((x) <= (y) ? (x) : (y))
#define max(x,y)  ((x) >= (y) ? (x) : (y))


UDPSend::UDPSend() {
	packetnum=0;
	sock=0;
}

void UDPSend::init(const char *address, int port, int recv_port) {
	if( sock ) close(sock);
	
	sock = socket( PF_INET, SOCK_DGRAM, 0 );

	addr.sin_family = AF_INET;  
	addr.sin_addr.s_addr = inet_addr(address);
	addr.sin_port = htons( port );

	sockaddr_in bind_addr{};
	bind_addr.sin_family = AF_INET;  
	bind_addr.sin_port = htons( recv_port );

	int ret = bind(
		sock,
		reinterpret_cast<const sockaddr *>(&bind_addr),
		sizeof(bind_addr)
	);
	assert(ret == 0);

	std::cout << "Listening to port " << port << "\n";
}

int UDPSend::send(
	const uint8_t *buffer,
	size_t len,
	int width,
	int height
) {
	uint8_t sendbuffer[MAX_DATA];
	FramePktHdr* header = reinterpret_cast<FramePktHdr*>(sendbuffer);
	uint8_t* frame_data = sendbuffer + sizeof(FramePktHdr);
	
	packetnum++;
	
	header->type = SERVER_FRAME;	
	header->time = clock() / (float)CLOCKS_PER_SEC;
	header->num_frame = packetnum;
	header->total_fragments = len / MAX_RAW_DATA;
	header->num_frag = 1;
	
	// If we don't completely fill out the last fragment,
	// increase the fragment count by 1
	if (len - header->total_fragments * MAX_RAW_DATA > 0)
		header->total_fragments++;
	
	ssize_t ret{};
	size_t bytes{};
	for(uint16_t i = 0; i < header->total_fragments; i++) {
		size_t raw_data_len = min(MAX_RAW_DATA, len - bytes);
		size_t data_len = raw_data_len + sizeof(FramePktHdr);
		std::copy(
			buffer + bytes,
			buffer + bytes + raw_data_len,
			frame_data
		);

		ret = sendto(
			sock,
			sendbuffer,
			data_len,
			0, 
			reinterpret_cast<const struct sockaddr *>(&addr),
			sizeof(addr)
		);

        if(ret == -1 || ret != data_len) {
			return ret;
		}

		bytes += raw_data_len;
		header->num_frag += 1;
	}

	assert(bytes == len);
	return bytes;
}

void UDPSend::send_sound(Sound sound) {
	SoundMessage msg { SERVER_SOUND, sound };
	sendto(
		sock,
		&msg,
		sizeof(msg),
		0, 
		reinterpret_cast<const struct sockaddr *>(&addr),
		sizeof(addr)
	);
}

void UDPSend::send_music(Music music) {
	MusicMessage msg { SERVER_MUSIC, music };
	sendto(
		sock,
		&msg,
		sizeof(msg),
		0, 
		reinterpret_cast<const struct sockaddr *>(&addr),
		sizeof(addr)
	);
}

ssize_t UDPSend::recv(uint8_t* buffer, size_t len) {
	return recvfrom(sock, buffer, len, 0, 0, 0);
}

void UDPSend::closeSock() {
	if (sock) {
		shutdown(sock, SHUT_RDWR);
		close( sock );
	}
	sock=0;
}
