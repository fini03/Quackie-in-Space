#pragma once

#include <box2d/b2_math.h>
#include <glm/glm.hpp>
#include "eventlisteners/Collider.hpp"
#include "eventlisteners/InterpolatedObject.hpp"

class b2Body;
class b2World;
class BlockManager;
class Player;
class MyVulkanEngine;

class Block : public Collider, public InterpolatedObject {
private:
    BlockManager* m_manager;
    Player* m_player;
    MyVulkanEngine* m_vulkan_engine;
    glm::vec3 starting_position{};
    b2Vec2 STARTING_POSITION{};
public:
    Block(b2World* world, const glm::vec3& position, BlockManager* manager, Player* player, MyVulkanEngine* mve);
    ~Block();

    bool is_colliding = false;

    void onInterpolation(float frame_dt, float ratio) override;
    void reset() override;

    bool beginCollisionPlayer(b2Body* player) override;
    bool endCollisionPlayer(b2Body* player) override;

    void pickup();
    void drop(const glm::vec3& new_position);
};
