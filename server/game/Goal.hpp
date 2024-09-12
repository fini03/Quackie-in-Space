#pragma once

#include <glm/glm.hpp>
#include "eventlisteners/Collider.hpp"

class b2World;
class b2Body;
class GameManager;
class Player;
class MyVulkanEngine;

namespace ve {
    class VESceneNode;
}

class Goal : public Collider {
private:
    GameManager* m_game;
    b2Body* m_body{};
    ve::VESceneNode* m_node;
    Player* m_player;
    MyVulkanEngine* m_vulkan_engine;

public:
    Goal(b2World* world, const glm::vec3& position, GameManager* game_manager, Player* player, MyVulkanEngine* mve);
    ~Goal();

    bool beginCollisionPlayer(b2Body* player) override;
};
