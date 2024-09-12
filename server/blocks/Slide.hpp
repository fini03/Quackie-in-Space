#pragma once

#include <box2d/b2_math.h>
#include <glm/glm.hpp>
#include "eventlisteners/Collider.hpp"
#include "eventlisteners/InterpolatedObject.hpp"

class b2World;
class b2Body;
class GameManager;

class Slide : public Collider, public InterpolatedObject {
private:
    b2Vec2 STARTING_POSITION;
    glm::vec3 starting_position;

    const float MOVE_DISTANCE = 5.0;
    const float MOVE_DURATION = 3.5;
    float current_distance = 0.0f;

public:
    Slide(b2World* world, const glm::vec3& position);
    ~Slide();
    
    bool is_slide_moving = false;

    void move_slide(const float dt);
    void move_slide_back(float velocity_x);

    void onPhysicsStep(float dt) override;
    void reset() override;

    bool beginCollisionPlayer(b2Body* player) override;
    bool endCollisionPlayer(b2Body* player) override;
};
