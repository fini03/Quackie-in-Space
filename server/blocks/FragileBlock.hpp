#pragma once

#include <box2d/b2_math.h>
#include <glm/glm.hpp>
#include "eventlisteners/Collider.hpp"
#include "eventlisteners/InterpolatedObject.hpp"

class b2World;
class b2Body;
class GameManager;

class FragileBlock : public Collider, public InterpolatedObject {
    private:
        glm::vec3 starting_position;
        float maximum_y_blocks_down = 1.05f;
        void stop_movement();
        float respawn_timer = 1.0f;
        b2Vec2 STARTING_POSITION{};
    public:
        FragileBlock(b2World* world, const glm::vec3& position);
        ~FragileBlock();

        bool is_triggered = false;
        float falling_countdown = 0.3;

        void respawn();
        void move_fragile_block();
        void onPhysicsStep(float dt) override;
        void reset() override;

        bool beginCollisionPlayer(b2Body* player) override;
};
