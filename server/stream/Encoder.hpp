#pragma once

#include <cstdint>

class AVCodec;
class AVCodecContext;
class AVPacket;
class AVFrame;
class AVBufferRef;
class AVHWFramesContext;
struct SwsContext;

class Encoder {
private:
    const AVCodec* m_codec;
    const bool m_hardware;

    AVCodecContext* m_ctxt;
    AVBufferRef* m_hw_ctxt;
    AVBufferRef* m_hw_frames_ref;
    AVHWFramesContext* m_frames_ctx;
    AVPacket* m_pkt;
    AVFrame* m_frame;
    AVFrame* m_hw_frame;
    SwsContext* m_sws;
    uint8_t* m_data;

public:
    Encoder(int width, int height, bool hardware = true);
    ~Encoder();
    void encode(uint8_t*& pkt_data, std::size_t& pkt_len); 
    void deref_pkt();

    uint8_t* data() const;

    const int width;
    const int height;
};
