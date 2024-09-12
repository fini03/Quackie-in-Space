#pragma once

#include "Trap.hpp"
#include <glm/glm.hpp>

class b2Body;
class b2World;
class GameManager;

class Chainsaw : public Trap {
    private:
        float maximum_y_blocks_down = 4.0f; // Maximum distance the chainsaw should move down
        float speed = 2.0f; // Speed of the chainsaw movement
        bool move_down; // Indicates the current direction of the chain
        glm::vec3 starting_position;

        void move_saw();
    public:
        Chainsaw(b2World* world, const glm::vec3& position, GameManager* game_manager, bool move_down);
        ~Chainsaw();
        
        bool beginCollisionPlayer(b2Body* player) override;
        void onPhysicsStep(float dt) override;
        void onInterpolation(float frame_dt, float ratio) override;
        void reset() override;
};
