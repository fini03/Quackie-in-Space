#include "TrapManager.hpp"
#include "Chainsaw.hpp"
#include "Fire.hpp"
#include "LaserBeam.hpp"
#include "box2d/b2_body.h"
#include <vector>

TrapManager::TrapManager(b2World* world, GameManager* game_manager) : m_world{ world }, m_game{game_manager} {}

TrapManager::~TrapManager() {
    traps.clear();
}

void TrapManager::create_fire_block(const glm::vec3& position) {
    Fire* fire_block = new Fire{m_world, position, m_game};
    traps.push_back(fire_block);
}

void TrapManager::create_laserbeam(const glm::vec3& position, bool enable_laser) {
    LaserBeam* laserbeam = new LaserBeam{m_world, position, m_game, enable_laser};
    traps.push_back(laserbeam);
}


void TrapManager::create_saw(const glm::vec3& position, bool move_down) {
    Chainsaw* saw = new Chainsaw{m_world, position, m_game, move_down};
    traps.push_back(saw);
}

void TrapManager::savePreviousState() {
    for(Trap* trap : traps) {
        trap->savePreviousState();
    }
}

void TrapManager::onPhysicsStep(float dt) {
    for(Trap* trap : traps) {
        trap->onPhysicsStep(dt);
    }
}

void TrapManager::onInterpolation(float frame_dt, float ratio) {
    for(Trap* trap : traps) {
        trap->onInterpolation(frame_dt, ratio);
    }
}

void TrapManager::reset() {
    for (Trap* trap : traps) {
        trap->reset();
    }
}

void TrapManager::deleteNode() {
    for (Trap* trap : traps) {
        delete trap;
    }
    traps.clear();
}

const std::vector<Trap*>& TrapManager::get_traps() const {
    return traps;
}
