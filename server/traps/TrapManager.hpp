#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "eventlisteners/PhysicsListener.hpp"

class b2World;
class b2Body;
class GameManager;
class Trap;

class TrapManager : public PhysicsListener {
    private:
        b2World* m_world;
        GameManager* m_game;
        std::vector<Trap*> traps;

    public:
        TrapManager(b2World* world, GameManager* game_manager);
        ~TrapManager();

        void create_fire_block(const glm::vec3& position);
        void create_laserbeam(const glm::vec3& position, bool enable_laser);
        void create_saw(const glm::vec3& position, bool move_down);

        void savePreviousState() override;
        void onPhysicsStep(float dt) override;
        void onInterpolation(float frame_dt, float ratio) override;
        void reset() override;
        void deleteNode() override;

        const std::vector<Trap*>& get_traps() const;
};
