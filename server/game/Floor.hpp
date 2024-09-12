#pragma once

#include <box2d/b2_math.h>
#include <glm/fwd.hpp>
#include "eventlisteners/Collider.hpp"

class b2World;
class GameManager;
class b2Body;

class Floor : public Collider {
private:
    b2Body* body{};
    GameManager* m_game;

public:
    Floor(b2World* world, GameManager* game_manager);
    ~Floor();

    bool beginCollisionPlayer(b2Body* player) override;
};
