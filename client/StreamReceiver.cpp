#include "StreamReceiver.hpp"
#include "Decoder.hpp"
#include "Network.hpp"
#include <iostream>

StreamReceiver::StreamReceiver(Network* network)
    : network{network}, decoder{nullptr}, m_rcvd_frame{} {
    decoder = new Decoder();
}

StreamReceiver::~StreamReceiver() {
    delete decoder;
};

void StreamReceiver::receive_stream(
    DecodedFrame& decoded_frame,
    std::mutex& frame_mutex
) {
    if (network->receive(&m_rcvd_frame)) {
    } else {
        std::cerr << "Received invalid frame data\n";
        return;
    }

    decoder->decode(m_rcvd_frame, decoded_frame, frame_mutex); 
}
