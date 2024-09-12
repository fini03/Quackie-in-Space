#pragma once

#include <VEInclude.h>
#include <mutex>
#include <vector>
#include "NetworkDefines.hpp"

class AVCodecContext;
class AVCodec;
class AVFrame;
class AVPacket;
class SwsContext;
class UDPSend;
class Encoder;
class MyVulkanEngine;
class GameManager;

class Stream : public ve::VEEventListener {
private:
    MyVulkanEngine* m_vulkan_engine;
    GameManager* m_game;
    VkBuffer stagingBuffer;
    VmaAllocation stagingBufferAllocation;

    float frame_delta;
    Encoder* encoder;

    void encode();

    std::mutex queue_mutex;
    std::vector<Sound> queue_sound;
    std::vector<Music> queue_music;

protected:
    virtual bool onKeyboard(ve::veEvent event) override;
    virtual void onFrameStarted(ve::veEvent event) override;
    virtual void onFrameEnded(ve::veEvent event) override;
        
public:
    Stream(MyVulkanEngine*, GameManager*);
    virtual ~Stream();

    UDPSend* sender;

    void push_sound(Sound sound);
    void push_music(Music music);
};
