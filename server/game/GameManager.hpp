#pragma once

#include <glm/fwd.hpp>
#include "eventlisteners/PhysicsListener.hpp"

class Key;
class Goal;
class b2World;
class Player;
class MyVulkanEngine;

enum EGameState { START, LOST, RUNNING, LOST_LIFE, LEVEL_COMPLETED, WON, EXIT };

class GameManager : public PhysicsListener {
    private:
        b2World* m_world;
        Key* key;
        Goal* goal;
        Player* m_player;
        MyVulkanEngine* m_vulkan_engine;

    public:
        GameManager(b2World* world, Player* player, MyVulkanEngine* mve);
        ~GameManager();

        void onPhysicsStep(float dt) override;
        void reset() override;
        void deleteNode() override;

        EGameState m_game_state = EGameState::RUNNING;
        int DUCK_LIVES = 3;
        bool reset_game = false;
        int CURRENT_LEVEL_NUMBER = 1;

        void create_key(const glm::vec3& position);
        void create_goal(const glm::vec3& position);
        void take_hit();
        Key* get_key() { return key; };
};
