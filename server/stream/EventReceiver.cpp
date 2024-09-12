#include <thread>
#include "NetworkDefines.hpp"
#include "UDPSend.hpp"
#include "EventReceiver.hpp"

using ve::veEvent;

static int sdl_to_glfw_key(int sdl_key) {
    if (sdl_key >= 'a' && sdl_key <= 'z')
        return (sdl_key - 'a') + GLFW_KEY_A;
    return sdl_key;
}

EventReceiver::EventReceiver(UDPSend* socket)
    : ve::VEEventListener("EventReceiver"), socket(socket),
    running(true) {}

EventReceiver::~EventReceiver() {
    socket->closeSock();
    recv_thread.join();
}

void EventReceiver::start_listening() {
    recv_thread = std::thread([&] {
        const size_t MAX_SIZE = sizeof(ClientMsgStats);
        uint8_t buffer[MAX_SIZE];

        while (running) {
            ssize_t ret = socket->recv(buffer, MAX_SIZE);
            if (ret <= 0)
                break;
            
            handle_msg(buffer);
        }
    });
}

void EventReceiver::handle_msg(const uint8_t* buffer) {
    ve::veEvent event{veEvent::VE_EVENT_NONE};
    const auto msg = reinterpret_cast<const ClientMsg*>(buffer);
    const auto key_msg = reinterpret_cast<const ClientMsgKey*>(buffer);

    switch (msg->type) {
    case CLIENT_CONNECT:
        break;
    case CLIENT_STATS:
        break;
    case CLIENT_KEYDOWN:
        event.subsystem = veEvent::VE_EVENT_SUBSYSTEM_GLFW;
        event.type = veEvent::VE_EVENT_KEYBOARD;
        event.idata1 = sdl_to_glfw_key(key_msg->key);
        event.idata3 = GLFW_PRESS;
        break;
    case CLIENT_KEYUP:
        event.subsystem = veEvent::VE_EVENT_SUBSYSTEM_GLFW;
        event.type = veEvent::VE_EVENT_KEYBOARD;
        event.idata1 = sdl_to_glfw_key(key_msg->key);
        event.idata3 = GLFW_RELEASE;
        break;
    default:
        std::cerr
            << "Unknown event received: "
            << (int)msg->type 
            << std::endl;
    }

    if (event.type != veEvent::VE_EVENT_NONE) {
        std::scoped_lock lock{queue_lock};
        event_queue.push_back(std::move(event));
    }
}

void EventReceiver::onFrameStarted(ve::veEvent /* event */) {
    ve::VEEngine* engine = ve::getEnginePointer();

    std::scoped_lock lock{queue_lock};
    for (const auto& event : event_queue)
        engine->addEvent(event);
    event_queue.clear();
}
