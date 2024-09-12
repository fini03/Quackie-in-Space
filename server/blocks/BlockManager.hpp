#pragma once

#include <VEInclude.h>
#include <vector>
#include <glm/glm.hpp>
#include "eventlisteners/PhysicsListener.hpp"

class b2World;
class Block;
class Player;
class MyVulkanEngine;

class BlockManager : public PhysicsListener, public ve::VEEventListener {
private:
    std::vector<Block*> blocks;
    b2World* m_world;
    Player* m_player;
    MyVulkanEngine* m_vulkan_engine;
protected:
    bool onKeyboard(ve::veEvent event) override;

public:
    BlockManager(b2World* world, Player* player, MyVulkanEngine* mve);
    ~BlockManager();

    bool is_E_key_pressed = false;
    bool is_space_key_pressed = false;

    void create_block(const glm::vec3& position);
    std::vector<Block*> get_blocks() const;

    void savePreviousState() override;
    void onPhysicsStep(float dt) override;
    void onInterpolation(float frame_dt, float ratio) override;
    void reset() override;
    void deleteNode() override;
};
