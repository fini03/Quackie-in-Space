#pragma once

#include <box2d/b2_math.h>
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

class Key : public Collider {
private:
    b2Body* m_body;
    ve::VESceneNode* m_node;
    Player* m_player;
    MyVulkanEngine* m_vulkan_engine;
    b2Vec2 STARTING_POSITION{};
public:
    Key(b2World* world, const glm::vec3& position, Player* player, MyVulkanEngine* mve);
    ~Key();

    void onPhysicsStep();
    bool beginCollisionPlayer(b2Body* player) override;
    void reset();
};
