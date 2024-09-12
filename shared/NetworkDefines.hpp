#pragma once
#include <cstdint>
extern "C" {
#include <unistd.h>
}

enum ServerMsgType : uint8_t {
    SERVER_FRAME,
    SERVER_SOUND,
    SERVER_MUSIC
};

struct FramePktHdr {
    ServerMsgType type;
    uint32_t time;
    uint16_t num_frame;
    uint16_t total_fragments;
    uint16_t num_frag;
};

enum Sound : uint8_t {
    SOUND_GAME_LOST,
    SOUND_GAME_WON,
    SOUND_PICKUP_KEY,
    SOUND_PICKUP_BLOCK,
    SOUND_DROP_BLOCK,
    SOUND_LOST_LIFE,
    SOUND_LEVEL_COMPLETED,
    SOUND_COUNT
};

enum Music : uint8_t {
    MUSIC_BACKGROUND,
    MUSIC_COUNT
};

struct SoundMessage {
    ServerMsgType type;
    Sound sound;
};

struct MusicMessage {
    ServerMsgType type;
    Music sound;
};

struct FrameStats {
    uint32_t time; // Time passed in milliseconds
    uint32_t nbytes_received;
    uint32_t npackets_lost;
    uint32_t nframes;
};

const std::size_t MAX_RAW_DATA = 65000;
const std::size_t MAX_DATA = MAX_RAW_DATA + sizeof(FramePktHdr);
const std::size_t MAX_FRAME_SIZE = MAX_RAW_DATA
    * ((1 << (sizeof(FramePktHdr::total_fragments) * 8)) - 1);

struct ReceivedFrame {
    size_t size;
    uint32_t time;
    uint8_t data[MAX_FRAME_SIZE];
};

enum ClientMsgType : uint8_t {
    CLIENT_CONNECT,
    CLIENT_STATS,
    CLIENT_KEYDOWN,
    CLIENT_KEYUP
};

struct ClientMsg {
    ClientMsgType type;
};

struct ClientMsgStats {
    ClientMsgType type;
    uint32_t time_ms;
    uint32_t nbytes_received;
    uint32_t npackets_lost;
    uint32_t nframes;
};

struct ClientMsgKey {
    ClientMsgType type;
    int32_t key;
};
