#pragma once

#include "Trap.hpp"
#include <glm/glm.hpp>

class b2Body;
class b2World;
class GameManager;

class LaserBeam : public Trap {
    public:
        LaserBeam(b2World* world, const glm::vec3& position, GameManager* game_manager, bool enable_laser);
        ~LaserBeam();

        bool beginCollisionPlayer(b2Body* player) override;

        void savePreviousState() override;
        void onInterpolation(float frame_dt, float ratio) override;
        void reset() override;
};
