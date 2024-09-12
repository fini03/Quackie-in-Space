#include <iostream>
#include <chrono>
#include <libavutil/pixfmt.h>
#include <source_location>
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/opt.h>
    #include <libavutil/hwcontext.h>
    #include <libswscale/swscale.h>
}
#include "Encoder.hpp"

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

Encoder::Encoder(const int width, const int height, bool hardware)
    : m_hardware{hardware}, width{width}, height{height}
{
    if (m_hardware) {
        m_hw_ctxt = nullptr;
        ffchk(av_hwdevice_ctx_create(
            &m_hw_ctxt,
            AV_HWDEVICE_TYPE_VAAPI,
            nullptr,
            nullptr,
            0
        ));

        ffchk(m_codec = avcodec_find_encoder_by_name("h264_vaapi"));
    } else{
        ffchk(m_codec = avcodec_find_encoder_by_name("libx264"));
    }
    ffchk(m_ctxt = avcodec_alloc_context3(m_codec));
    ffchk(m_pkt = av_packet_alloc());
    ffchk(m_frame = av_frame_alloc());

    // Make sure to set ultra low latency if we have H264
    if (m_codec->id == AV_CODEC_ID_H264) {
        av_opt_set(m_ctxt->priv_data, "preset", "ultrafast", 0);
        av_opt_set(m_ctxt->priv_data, "tune", "zerolatency", 0);
    }

    // Set the relevant codec parameters
    m_ctxt->bit_rate = 10'000'000;
    m_ctxt->width = width;
    m_ctxt->height = height;
    m_ctxt->time_base = (AVRational){1, 60};
    m_ctxt->framerate = (AVRational){60, 1};
    m_ctxt->pix_fmt = m_hardware ? AV_PIX_FMT_VAAPI : AV_PIX_FMT_YUV420P;
    m_ctxt->gop_size = 5;

    // So for the latency right now, kicking the b-frames is pretty
    // much the only option to satisfy the latency requirements of the
    // game
    m_ctxt->max_b_frames = 0; 

    // Initialize the hardware frame context if we are doing
    // hardware encoding
    if (m_hardware) {
        ffchk(m_hw_frames_ref = av_hwframe_ctx_alloc(m_hw_ctxt));
        m_frames_ctx = reinterpret_cast<AVHWFramesContext*>(
            m_hw_frames_ref->data);
        m_frames_ctx->format = AV_PIX_FMT_VAAPI;
        m_frames_ctx->sw_format = AV_PIX_FMT_NV12;
        m_frames_ctx->width = width;
        m_frames_ctx->height = height;
        m_frames_ctx->initial_pool_size = 20;
        ffchk(av_hwframe_ctx_init(m_hw_frames_ref));
        m_ctxt->hw_frames_ctx = av_buffer_ref(m_hw_frames_ref);
        av_buffer_unref(&m_hw_frames_ref);
    }

    ffchk(avcodec_open2(m_ctxt, m_codec, nullptr));

    // Copy parameters from codec to frame
    m_frame->format = m_hardware ? AV_PIX_FMT_NV12 : m_ctxt->pix_fmt;
    m_frame->width = m_ctxt->width;
    m_frame->height = m_ctxt->height;
    m_data = new unsigned char[m_frame->width * m_frame->height * 4];
    ffchk(av_frame_get_buffer(m_frame, 0));

    // Allocate a conversion context
    m_sws = sws_getContext(
        m_frame->width,
        m_frame->height,
        AV_PIX_FMT_BGRA,
        m_frame->width,
        m_frame->height,
        m_hardware ? AV_PIX_FMT_NV12 : m_ctxt->pix_fmt,
        0,
        0,
        0,
        0
    );

}

Encoder::~Encoder() {
    sws_freeContext(m_sws);
    av_frame_free(&m_frame);
    av_packet_free(&m_pkt);
    avcodec_free_context(&m_ctxt);

    if (m_hardware) {
        av_buffer_unref(&m_hw_ctxt);
    }
}

uint8_t* Encoder::data() const {
    return m_data;
}

void Encoder::encode(uint8_t*& pkt_data, std::size_t& pkt_len) {
    ffchk(av_frame_make_writable(m_frame));

    // Convert RGB to YUV
    uint8_t* data[1] = { m_data };
    int linesize[1] = { 4 * m_frame->width };

    sws_scale(
        m_sws,
        data,
        linesize,
        0,
        m_frame->height,
        m_frame->data,
        m_frame->linesize
    );

    // Transfer data for hardware frame encoding
    if (m_hardware) {
        ffchk(m_hw_frame = av_frame_alloc());
        ffchk(av_hwframe_get_buffer(m_ctxt->hw_frames_ctx, m_hw_frame, 0));
        ffchk(m_hw_frame->hw_frames_ctx);
        ffchk(av_hwframe_transfer_data(m_hw_frame, m_frame, 0));
    }

    // Send frame to encoder
    ffchk(avcodec_send_frame(m_ctxt, m_hardware ? m_hw_frame : m_frame));

    // Read packet from encoder
    int ret = avcodec_receive_packet(m_ctxt, m_pkt);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        pkt_len = 0;
        return;
    }
    ffchk(ret);

    // Pass pointer of packet data for parent to send (and parent
    // also needs to call deref then at some point)
    pkt_data = m_pkt->data;
    pkt_len = m_pkt->size;
}

void Encoder::deref_pkt() {
    if (m_hardware)
        av_frame_free(&m_hw_frame);
    av_packet_unref(m_pkt);
}
