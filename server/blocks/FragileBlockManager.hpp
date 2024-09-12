#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "eventlisteners/PhysicsListener.hpp"

class FragileBlock;
class b2World;
class b2Body;
class GameManager;

class FragileBlockManager : public PhysicsListener {
private:
    std::vector<FragileBlock*> fragile_blocks;
    b2World* m_world;

public:
    FragileBlockManager(b2World* world);
    ~FragileBlockManager();

    void create_fragile_block(const glm::vec3& position);

    void savePreviousState() override;
    void onPhysicsStep(float dt) override;
    void onInterpolation(float frame_dt, float ratio) override;
    void reset() override;
    void deleteNode() override;
};


