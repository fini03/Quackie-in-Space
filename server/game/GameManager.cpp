#include "GameManager.hpp"
#include "Key.hpp"
#include "Goal.hpp"
#include "game/MyVulkanEngine.hpp"
#include "game/Player.hpp"

GameManager::GameManager(b2World* world, Player* player, MyVulkanEngine* mve) : m_world{ world }, m_player{ player }, m_vulkan_engine{mve} {}

GameManager::~GameManager() {}

void GameManager::onPhysicsStep(float /* dt */) {
    key->onPhysicsStep();
}

void GameManager::create_key(const glm::vec3& position) {
    key = new Key{m_world, position, m_player, m_vulkan_engine};
}

void GameManager::create_goal(const glm::vec3& position) {
    goal = new Goal{m_world, position, this, m_player, m_vulkan_engine};
}

void GameManager::reset() {
    if(key != nullptr)
        key->reset();
}

void GameManager::deleteNode() {
    delete key;
    delete goal;

    key = nullptr;
    goal = nullptr;
}

void GameManager::take_hit() {
    if (m_game_state == EGameState::RUNNING) {
        DUCK_LIVES--;

        if (DUCK_LIVES == 0) {
            m_game_state = EGameState::LOST;
            m_vulkan_engine->push_sound(SOUND_GAME_LOST);
        }
        else {
            m_game_state = EGameState::LOST_LIFE;
            m_vulkan_engine->push_sound(SOUND_LOST_LIFE);
        }
    }
}
