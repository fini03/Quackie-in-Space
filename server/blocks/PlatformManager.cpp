#include <vector>
#include "PlatformManager.hpp"
#include "Platform.hpp"

PlatformManager::PlatformManager(b2World* world) : m_world { world } {}

PlatformManager::~PlatformManager() {
    platforms.clear();
}

void PlatformManager::savePreviousState() {
    for (Platform* platform : platforms) {
        platform->savePreviousState();
    }
}

void PlatformManager::onPhysicsStep(float dt) {
    for (Platform* platform : platforms) {
        platform->onPhysicsStep(dt);
    }
}

void PlatformManager::onInterpolation(float dt, float ratio) {
    for (Platform* platform : platforms) {
        platform->onInterpolation(dt, ratio);
    }
}

void PlatformManager::reset() {
    for (Platform* platform : platforms) {
        platform->reset();
    }
}

void PlatformManager::deleteNode() {
    for(Platform* platform : platforms)
        delete platform;

    platforms.clear();
}

void PlatformManager::create_platform(const glm::vec3& position) {
    Platform* platform = new Platform{m_world, position};
    platforms.push_back(platform);
}
