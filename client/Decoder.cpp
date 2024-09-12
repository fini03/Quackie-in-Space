#include <iostream>
#include <source_location>
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/opt.h>
}
#include "Decoder.hpp"
#include "NetworkDefines.hpp"

// Quick exit function for checking ffmpeg function call results
static void ffchk(
    const int ret,
    const std::source_location loc = std::source_location::current()
) {
    if (ret < 0) {
        av_log(
            nullptr,
            AV_LOG_ERROR,
            "%s(%d:%d), %s: %s\n",
            loc.file_name(),
            loc.line(),
            loc.column(),
            loc.function_name(),
            av_err2str(ret)
        );
        std::exit(1);
    }
}

// Quick exit function for checking ffmpeg function call results
template <typename T>
static void ffchk(
    T* ret,
    const std::source_location loc = std::source_location::current()
) {
    if (ret == 0) {
        av_log(
            nullptr,
            AV_LOG_ERROR,
            "%s(%d:%d), %s\n",
            loc.file_name(),
            loc.line(),
            loc.column(),
            loc.function_name()
        );
        std::exit(1);
    }
}

#define FFCHK_NOEXIT(x)                      \
    {                                        \
        std::source_location loc =           \
            std::source_location::current(); \
        int ret = x;                         \
        if (ret < 0)                         \
        {                                    \
            av_log(                          \
                nullptr,                     \
                AV_LOG_ERROR,                \
                "%s(%d:%d), %s: %s\n",       \
                loc.file_name(),             \
                loc.line(),                  \
                loc.column(),                \
                loc.function_name(),         \
                av_err2str(ret)              \
            );                               \
            return;                          \
        }                                    \
    }

Decoder::Decoder() {
    ffchk(m_codec = avcodec_find_decoder(AV_CODEC_ID_H264));
    ffchk(m_ctxt = avcodec_alloc_context3(m_codec));
    ffchk(m_pkt = av_packet_alloc());
    ffchk(m_frame = av_frame_alloc());

    // Set the relevant codec parameters
    m_ctxt->time_base = (AVRational){1, 60};
    m_ctxt->framerate = (AVRational){60, 1};
    ffchk(avcodec_open2(m_ctxt, m_codec, nullptr));

    ffchk(m_parser = av_parser_init(m_codec->id));
}

Decoder::~Decoder() {
    avcodec_free_context(&m_ctxt);
    av_frame_free(&m_frame);
    av_packet_free(&m_pkt);
}

void Decoder::decode(
    const ReceivedFrame& rcvd_frame,
    DecodedFrame& decoded_frame,
    std::mutex& frame_mutex
) {
    size_t data_size = rcvd_frame.size;
    decoded_frame = {};

    while (data_size > 0) {
        int r = av_parser_parse2(
            m_parser,
            m_ctxt,
            &m_pkt->data,
            &m_pkt->size,
            rcvd_frame.data,
            rcvd_frame.size,
            AV_NOPTS_VALUE,
            AV_NOPTS_VALUE,
            0
        );
        FFCHK_NOEXIT(r);

        data_size -= r;

        if (m_pkt->size == 0)
            continue;

        FFCHK_NOEXIT(avcodec_send_packet(m_ctxt, m_pkt));

        // With using YUV we dont need to do the sws scale
        // because sws scale was used to convert from YUV 
        // to RGB
        std::scoped_lock lock{frame_mutex};
        while ((r = avcodec_receive_frame(m_ctxt, m_frame)) == 0) {
            decoded_frame.width = m_frame->width;
            decoded_frame.height = m_frame->height;
            decoded_frame.Yplane = m_frame->data[0];
            decoded_frame.Ysize = m_frame->linesize[0];
            decoded_frame.Uplane = m_frame->data[1];
            decoded_frame.Usize = m_frame->linesize[1];
            decoded_frame.Vplane = m_frame->data[2];
            decoded_frame.Vsize = m_frame->linesize[2];
        }

        if (r != AVERROR(EAGAIN) && r != AVERROR_EOF) {
            char errBuf[AV_ERROR_MAX_STRING_SIZE];
            av_strerror(r, errBuf, sizeof(errBuf));
            std::cerr
                << "Error during decoding: "
                << errBuf
                << std::endl;
        }
    }
}
