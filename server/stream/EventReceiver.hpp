#include <VEInclude.h>
#include <mutex>
#include <vector>

class UDPSend;

class EventReceiver : public ve::VEEventListener {
private:
    UDPSend* socket;

    bool running;
    std::thread recv_thread;
    std::mutex queue_lock;
    std::vector<ve::veEvent> event_queue;

    void handle_msg(const uint8_t* msg);
public:
    EventReceiver(UDPSend* socket);
    virtual ~EventReceiver();

    void start_listening();
    void onFrameStarted(ve::veEvent event) override;
};
