#pragma once

#include <box2d/b2_math.h>
#include <glm/glm.hpp>
#include "eventlisteners/Collider.hpp"
#include "eventlisteners/InterpolatedObject.hpp"

class b2World;
class b2Body;
class GameManager;

class Platform : public Collider, public InterpolatedObject {
private:
    glm::vec3 starting_position;
    b2Vec2 STARTING_POSITION{};

    const float MOVE_DISTANCE = 10.0;
    const float MOVE_DURATION = 3.5;
    float current_distance = 0.0f;

    void apply_velocity();
    void fall_down();
    void stop_movement();
public:
    Platform(b2World* world, const glm::vec3& position);
    ~Platform();

    bool is_platform_moving = false;

    void move_platform(const float dt);
    void onPhysicsStep(float dt) override;
    void reset() override;

    bool beginCollisionPlayer(b2Body* player) override;
    bool endCollisionPlayer(b2Body* player) override;
};
