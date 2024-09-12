#pragma once

#include <box2d/b2_math.h>
#include <glm/fwd.hpp>
#include "eventlisteners/Collider.hpp"
#include "eventlisteners/InterpolatedObject.hpp"

class b2World;
class ButtonManager;

class Button : public Collider, public InterpolatedObject {
private:
    ButtonManager* m_manager;
    int pressed_count;
    b2Vec2 STARTING_POSITION{};

public:
    Button(b2World* world, const glm::vec3& position, ButtonManager* manager);
    ~Button();

    void savePreviousState() override;
    void onPhysicsStep(float dt) override;
    void onInterpolation(float frame_dt, float ratio) override;
    void reset() override;

    bool beginCollisionPlayer(b2Body* player) override;
    bool endCollisionPlayer(b2Body* player) override;
    void beginCollisionNPC(b2Body* npc) override;
    void endCollisionNPC(b2Body* npc) override;
};
