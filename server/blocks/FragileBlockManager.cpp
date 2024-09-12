#include "FragileBlock.hpp"
#include "FragileBlockManager.hpp"
#include <vector>

FragileBlockManager::FragileBlockManager(b2World* world) : m_world { world } {}

FragileBlockManager::~FragileBlockManager() {
    fragile_blocks.clear();
}

void FragileBlockManager::savePreviousState() {
    for(FragileBlock* fragile_block : fragile_blocks) {
        fragile_block->savePreviousState();
    } 
}

void FragileBlockManager::onPhysicsStep(float dt) {
    for(FragileBlock* fragile_block : fragile_blocks) {
        fragile_block->onPhysicsStep(dt);
    } 
}

void FragileBlockManager::onInterpolation(float frame_dt, float ratio) {
    for(FragileBlock* fragile_block : fragile_blocks) {
        fragile_block->onInterpolation(frame_dt, ratio);
    } 
}

void FragileBlockManager::reset() {
    for(FragileBlock* fragile_block : fragile_blocks) {
        fragile_block->reset();
    } 
}

void FragileBlockManager::deleteNode() {
    for(FragileBlock* fragile_block : fragile_blocks)
        delete fragile_block;

    fragile_blocks.clear();
}

void FragileBlockManager::create_fragile_block(const glm::vec3& position) {
    FragileBlock* fragile_block = new FragileBlock{m_world, position};
    fragile_blocks.push_back(fragile_block);
}
