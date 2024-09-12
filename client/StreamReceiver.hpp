#include <mutex>
#include "NetworkDefines.hpp"

class Decoder;
class Network;
class SwsContext;

struct DecodedFrame;

class StreamReceiver {
    private:
        Network* network;
        Decoder* decoder;
        ReceivedFrame m_rcvd_frame;

    public:
        StreamReceiver(Network* network);
        virtual ~StreamReceiver();
        void receive_stream(
            DecodedFrame& decoded_frame,
            std::mutex& frame_mutex
        );
};
