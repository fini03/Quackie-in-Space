#include <chrono>
#include <cstring>
#include <iostream>
#include "Client.hpp"
#include "Network.hpp"
#include "NetworkDefines.hpp"

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::seconds;

Network::Network(Client* client, uint16_t port)
    : m_client{client},
    last_heartbeat{std::chrono::steady_clock::now()},
    last_stats{std::chrono::steady_clock::now()},
    m_connected(false)
{
    sock_fd = socket(PF_INET, SOCK_DGRAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    int ret = bind(
        sock_fd,
        reinterpret_cast<const sockaddr *>(&addr),
        sizeof(addr)
    );

    if(ret) {
        std::cerr << "Unable to set up receiver" << std::endl;
        exit(1);
    }

    std::cout << "Listening to port " << port << "\n";
}

Network::~Network() {
    std::cout << "Shutting down socket" << "\n";
    close(sock_fd);
}

bool Network::receive(ReceivedFrame* frame) {
    FramePktHdr* pkt_hdr = reinterpret_cast<FramePktHdr*>(recv_buffer);
    SoundMessage* snd_msg = reinterpret_cast<SoundMessage*>(recv_buffer);
    MusicMessage* msc_msg = reinterpret_cast<MusicMessage*>(recv_buffer);
    uint16_t frame_num{};
    uint16_t max_frags{2}; // Will be overwritten by valid header
    
    frame->size = 0;

    for (uint16_t next_frag = 1; next_frag <= max_frags; ) {
        // If we have leftover bytes from the previous receive,
        // don't recv but rather process the old message first
        if (nbytes_unread == 0) {
            socklen_t addr_len = sizeof(server_addr);
            nbytes_unread = recvfrom(
                sock_fd,
                recv_buffer,
                MAX_DATA,
                0,
                reinterpret_cast<sockaddr*>(&server_addr),
                &addr_len
            );
            stats.nbytes_received += nbytes_unread;
        }

        // Orderly shutdown
        if (nbytes_unread == 0)
            std::exit(0);

        // Some socket error
        if (nbytes_unread < 0) {
            std::cerr
                << "Error during recvfrom: "
                << std::strerror(errno)
                << std::endl;
            std::exit(1);
        }

        // Register heartbeat (we received an orderly packet from
        // the server, so the connection is still alive)
        {
            std::scoped_lock lock{heartbeat_lock};
            last_heartbeat = std::chrono::steady_clock::now();
            m_connected = true;
        }

        // Check if we received any non-frame update from server
        switch (pkt_hdr->type) {
        case SERVER_MUSIC:
            m_client->push_music_event(msc_msg->sound);
            nbytes_unread = 0;
            continue;
        case SERVER_SOUND:
            m_client->push_sound_event(snd_msg->sound);
            nbytes_unread = 0;
            continue;
        case SERVER_FRAME:
            /* Just process it afterwards */
            break;
        default:
            std::cerr
                << "Unknown packet type received: "
                << (int)pkt_hdr->type
                << std::endl;
            nbytes_unread = 0;
            return false;
        }

        // Make sure our packet is at least the size of the header
        if (nbytes_unread <= sizeof(FramePktHdr)) {
            std::cerr << "Received fragment with invalid header\n";
            nbytes_unread = 0;
            stats.npackets_lost++;
            return false;
        }

        // If it's the first frag, remember the frame num
        if (next_frag == 1)
            frame_num = pkt_hdr->num_frame;

        // If we get a different frame num, we lost some
        // fragments for the frame we wanted to receive
        if (frame_num != pkt_hdr->num_frame) {
            std::cerr
                << "Lost fragment at end: "
                << static_cast<uint32_t>(next_frag)
                << "\n";
            stats.npackets_lost++;
            return false;
        }

        // Fragment we received is from the same frame,
        // but not the fragment we expected
        if (next_frag != pkt_hdr->num_frag) {
            std::cerr
                << "Lost fragment in the middle: "
                << static_cast<uint32_t>(next_frag)
                << "\n";

            // Fragment we read is never going to be usable,
            // because we lost a middle fragment of the frame
            nbytes_unread = 0;  
            stats.npackets_lost++;
            return false;
        }

        // Fragment is useable, copy stuff and advance
        size_t frame_data_len = nbytes_unread - sizeof(FramePktHdr);
        std::copy(
            recv_buffer + sizeof(FramePktHdr), // Start after header
            recv_buffer + nbytes_unread, // Copy until end of buffer
            frame->data + frame->size // Frame data + what we read so far
        );
        frame->size += frame_data_len;

        max_frags = pkt_hdr->total_fragments;
        frame_num = pkt_hdr->num_frame;

        // We processed the full frame, no bytes unread anymore
        nbytes_unread = 0;
        next_frag++;
    }

    nbytes_unread = 0;
    return true;
}

void Network::send(const void* msg, size_t len) {
    sendto(
        sock_fd,
        msg,
        len,
        0,
        reinterpret_cast<const sockaddr*>(&server_addr),
        sizeof(server_addr)
    );
}

void Network::check_heartbeat() {
    // If the last packet we received from the server was more than
    // N seconds ago, try resending connection start packets
    const auto now = steady_clock::now();
    int delta;
    {
        std::scoped_lock lock{heartbeat_lock};
        delta = duration_cast<seconds>(now - last_heartbeat).count();
    }
    if (delta > 3)
        m_connected = false;

    if (!m_connected) {
        ClientMsg msg{ CLIENT_CONNECT };
        send(&msg, sizeof(msg));
    }
}

void Network::send_keydown(int key_code) {
    ClientMsgKey msg{ CLIENT_KEYDOWN, key_code };
    send(&msg, sizeof(msg));
}

void Network::send_keyup(int key_code) {
    ClientMsgKey msg{ CLIENT_KEYUP, key_code };
    send(&msg, sizeof(msg));
}
