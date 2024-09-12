#pragma once

#include <cstdint>
#include <mutex>

// Forward dec ffmpeg libs
class AVCodec;
class AVCodecContext;
class AVPacket;
class AVFrame;
class AVCodecParserContext;

struct ReceivedFrame;
struct DecodedFrame {
    int width;
    int height;
    uint8_t* Yplane;
    int Ysize;
    uint8_t* Uplane;
    int Usize;
    uint8_t* Vplane;
    int Vsize;
};

class Decoder {
private:
	const AVCodec* m_codec;
	AVCodecContext* m_ctxt;
	AVCodecParserContext* m_parser;
	AVFrame* m_frame;
	AVPacket* m_pkt;

public:
	Decoder();
	~Decoder();

	void decode(
		const ReceivedFrame& rcvd_frame,
		DecodedFrame& decoded_frame,
        std::mutex& frame_mutex
	);
};
