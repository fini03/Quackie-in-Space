#include "BlockManager.hpp"
#include "Block.hpp"
#include "game/MyVulkanEngine.hpp"
#include "game/Player.hpp"
#include <GLFW/glfw3.h>
#include <vector>
#include <VEInclude.h>

using namespace ve;

BlockManager::BlockManager(b2World* world, Player* player, MyVulkanEngine* mve) : VEEventListener("Block Manager"), m_world { world }, m_player{player}, m_vulkan_engine{mve} {}

BlockManager::~BlockManager() {
    blocks.clear();
}

bool BlockManager::onKeyboard(veEvent event) {
    switch (event.idata1) {
        case GLFW_KEY_E: // Pick up block
            if(event.idata3 == GLFW_PRESS) {
                is_E_key_pressed = true;
            } else if(event.idata3 == GLFW_RELEASE) {
                is_E_key_pressed = false;
            }
            break;
        case GLFW_KEY_SPACE: // Drop block
            if(event.idata3 == GLFW_PRESS) {
                is_space_key_pressed = true;
            } else if(event.idata3 == GLFW_RELEASE) {
                is_space_key_pressed = false;
            }
            break;
    }  
    return false;
}

void BlockManager::create_block(const glm::vec3& position) {
    Block* block = new Block{m_world, position, this, m_player, m_vulkan_engine};
    blocks.push_back(block);
}

void BlockManager::savePreviousState() {
    for(Block* block : blocks) {
        block->savePreviousState();
    }
}

void BlockManager::onPhysicsStep(float dt) {
    for(Block* block : blocks) {
        block->onPhysicsStep(dt);
    }
}

void BlockManager::onInterpolation(float frame_dt, float ratio) {
    for(Block* block : blocks) {
        block->onInterpolation(frame_dt, ratio);
    }
}

void BlockManager::reset() {
    for(Block* block : blocks) {
        block->reset();
    }
}

void BlockManager::deleteNode() {
    for(Block* block : blocks)
        delete block;

    blocks.clear();
}

std::vector<Block*> BlockManager::get_blocks() const {
    return blocks;
}
