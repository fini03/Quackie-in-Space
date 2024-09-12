#pragma once

#include "Trap.hpp"
#include <glm/glm.hpp>

class b2Body;
class b2World;
class GameManager;

class Fire : public Trap {
    public:
        Fire(b2World* world, const glm::vec3& position, GameManager* game_manager);
        ~Fire();

        bool beginCollisionPlayer(b2Body* player) override;

        void savePreviousState() override;
        void onInterpolation(float frame_dt, float ratio) override;
        void reset() override;
};
