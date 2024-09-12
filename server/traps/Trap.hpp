#pragma once

#include "eventlisteners/Collider.hpp"
#include "eventlisteners/InterpolatedObject.hpp"
#include <box2d/b2_math.h>
#include <glm/glm.hpp>

class b2World;
class b2Body;
class GameManager;

class Trap : public Collider, public InterpolatedObject {
protected:
    GameManager *m_game;
    b2Vec2 STARTING_POSITION{};

public:
    Trap(b2World* world, const glm::vec3& position, GameManager* game_manager);
    virtual ~Trap();

    bool can_collide = true;
    bool m_enable_laser = false;
};
