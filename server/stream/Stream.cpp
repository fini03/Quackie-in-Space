#include <GLFW/glfw3.h>
#include <source_location>
#include <VEInclude.h>
#include "Stream.hpp"
#include "Encoder.hpp"
#include "UDPSend.hpp"
#include "game/GameManager.hpp"
#include "game/MyVulkanEngine.hpp"

// Quick exit function for checking Vulkan function call results
static void vkchk(
    const VkResult r,
    const std::source_location loc = std::source_location::current()
) {
    if (r != VK_SUCCESS) {
        std::cerr
            << loc.file_name()
            << "("
            << loc.line()
            << "), "
            << loc.function_name()
            << ": "
            << r
            << std::endl;
        std::exit(1);
    }
}

Stream::Stream(MyVulkanEngine* mve, GameManager* game_manager) :
    VEEventListener("StreamListener"),
    m_vulkan_engine{mve},
    m_game{game_manager},
    frame_delta{0},
    encoder{nullptr}
{
    sender = new UDPSend();
    sender->init("127.0.0.1", 8080, 8081);
};

Stream::~Stream() {
    if (encoder) {
        delete encoder;
        encoder = nullptr;

        ve::VERenderer* renderer = ve::getEnginePointer()->getRenderer();
        VmaAllocator allocator = renderer->getVmaAllocator();
        vmaDestroyBuffer(allocator, stagingBuffer, stagingBufferAllocation);
    }
};

bool Stream::onKeyboard(ve::veEvent event) {
    VkExtent2D extent = ve::getWindowPointer()->getExtent();

    switch (event.idata1) {
        case GLFW_KEY_P:
            if (!encoder) {
                m_game->m_game_state = EGameState::RUNNING;
                m_vulkan_engine->push_music(MUSIC_BACKGROUND);
                // Requirements for ffmpeg codec width/height
                if (extent.width % 2 == 1) {
                    std::cerr << "Width not a multiple of 2" << std::endl;
                    return false;
                }
                if (extent.height % 2 == 1) {
                    std::cerr << "Height not a multiple of 2" << std::endl;
                    return false;
                }

                ve::VERenderer* renderer = ve::getEnginePointer()->getRenderer();
                VmaAllocator allocator = renderer->getVmaAllocator();
                uint32_t imageSize = extent.height * extent.width * 4;
                vkchk(vh::vhBufCreateBuffer(allocator, imageSize,
                    VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                    VMA_MEMORY_USAGE_GPU_TO_CPU,
                    &stagingBuffer, &stagingBufferAllocation));

                encoder = new Encoder(extent.width, extent.height);
            }
            break;
        case GLFW_KEY_ESCAPE:
            if (encoder) {
                delete encoder;
                encoder = nullptr;

                ve::VERenderer* renderer = ve::getEnginePointer()->getRenderer();
                VmaAllocator allocator = renderer->getVmaAllocator();
                vmaDestroyBuffer(allocator, stagingBuffer, stagingBufferAllocation);
                exit(0);
            }
            break;
    }

    return false;
}

void Stream::onFrameStarted(ve::veEvent /* event */) {
    std::scoped_lock lock{queue_mutex};
    for (const auto sound : queue_sound)
        sender->send_sound(sound);
    for (const auto music : queue_music)
        sender->send_music(music);
    queue_sound.clear();
    queue_music.clear();
}

void Stream::onFrameEnded(ve::veEvent event) {
    if (!encoder)
        return;

    if (frame_delta > 0.016f) {
        frame_delta = 0.0f;
        encode();
    }
    
    frame_delta += event.dt;
}

void Stream::encode() {
    ve::VERenderer* renderer = ve::getEnginePointer()->getRenderer();
    VkDevice device = renderer->getDevice();
    VkQueue graphicsQueue = renderer->getGraphicsQueue();
    VkCommandPool commandPool = renderer->getCommandPool();
    VkImage image = renderer->getSwapChainImage();
    VkImageAspectFlagBits aspect = VK_IMAGE_ASPECT_COLOR_BIT;
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
    VmaAllocator allocator = renderer->getVmaAllocator();
    VkImageLayout layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    uint32_t width = encoder->width;
    uint32_t height = encoder->height;
    uint32_t imageSize = height * width * 4;

	VkCommandBuffer commandBuffer = vh::vhCmdBeginSingleTimeCommands(
        device,
        commandPool
    );

	vkchk(vh::vhBufTransitionImageLayout(
        device, graphicsQueue, commandBuffer, image,
		format, aspect, 1, 1,
		layout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
    ));

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = aspect;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };

	vkCmdCopyImageToBuffer(
        commandBuffer, image,VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        stagingBuffer, 1, &region
    );

	vkchk(vh::vhBufTransitionImageLayout(
        device, graphicsQueue, commandBuffer, image, format, aspect,
        1, 1, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, layout
    ));

	vkchk(vh::vhCmdEndSingleTimeCommands(
        device, graphicsQueue, commandPool, commandBuffer
    ));

    void *data;
    vkchk(vmaMapMemory(allocator, stagingBufferAllocation, &data));
    memcpy(encoder->data(), data, (size_t)imageSize);
    vmaUnmapMemory(allocator, stagingBufferAllocation);

    uint8_t* pkt_data = nullptr;
    std::size_t pkt_len = 0;

    // Encode the image
    encoder->encode(pkt_data, pkt_len);

    // Send the encoded packet
    sender->send(pkt_data, pkt_len, width, height);

    // Deref the image
    encoder->deref_pkt();
}

void Stream::push_sound(Sound sound) {
    std::scoped_lock lock{queue_mutex};
    queue_sound.push_back(sound);
}

void Stream::push_music(Music music) {
    std::scoped_lock lock{queue_mutex};
    queue_music.push_back(music);
}
