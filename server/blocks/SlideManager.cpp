#include "SlideManager.hpp"
#include "Slide.hpp"
#include <vector>

SlideManager::SlideManager(b2World* world) : m_world { world } {}

SlideManager::~SlideManager() {
    slides.clear();
}

void SlideManager::savePreviousState() {
    for (Slide* slide : slides) {
        slide->savePreviousState();
    }
}

void SlideManager::onPhysicsStep(float dt) {
    for (Slide* slide : slides) {
        slide->onPhysicsStep(dt);
    }
}

void SlideManager::onInterpolation(float frame_dt, float ratio) {
    for (Slide* slide : slides) {
        slide->onInterpolation(frame_dt, ratio);
    }
}

void SlideManager::reset() {
    for (Slide* slide : slides) {
        slide->reset();
    }
}

void SlideManager::deleteNode() {
    for (Slide* slide : slides)
        delete slide;
    slides.clear();
}

void SlideManager::create_slide(const glm::vec3& position) {
    Slide* slide = new Slide{m_world, position};
    slides.push_back(slide);
}
