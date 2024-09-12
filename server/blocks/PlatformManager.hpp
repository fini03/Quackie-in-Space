#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "eventlisteners/PhysicsListener.hpp"

class Platform;
class b2World;
class b2Body;
class GameManager;

class PlatformManager : public PhysicsListener {
private:
    std::vector<Platform*> platforms;
    b2World* m_world;

public:
    PlatformManager(b2World* world);
    ~PlatformManager();

    void create_platform(const glm::vec3& position);
    void move_platforms(b2Body* duck);

    void savePreviousState() override;
    void onPhysicsStep(float dt) override;
    void onInterpolation(float frame_dt, float ratio) override;
    void reset() override;
    void deleteNode() override;
};
