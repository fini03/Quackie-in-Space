#include <box2d/box2d.h>
#include <VEInclude.h>
#include "Floor.hpp"
#include "VESceneManager.h"
#include "game/GameManager.hpp"

using namespace ve;

Floor::Floor(b2World* world, GameManager* manager) : m_game{manager} {
    b2BodyDef bodyDef{};
    bodyDef.type = b2_staticBody;

    VESceneNode* floor;
	VECHECKPOINTER(floor = getSceneManagerPointer()->loadModel("The Plane", "media/models/plane", "plane_t_n_s.obj", 0, getRoot()) );
	floor->setTransform(
        glm::scale(
        glm::translate(
            glm::vec3{ 0,0,0,}
        ),
        glm::vec3(1000.0f, 1.0f, 1000.0f)
        )
    );

    bodyDef.position.Set(0.0f, -0.5f); 
    body = world->CreateBody(&bodyDef);

    b2FixtureDef fixtureDef{};
    b2PolygonShape polygonShape;
    polygonShape.SetAsBox(1000.0, 0.5);
    fixtureDef.shape = &polygonShape;
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    body->CreateFixture(&fixtureDef);
    getSceneManagerPointer()->setVisibility(floor, false);
}

Floor::~Floor() {}

bool Floor::beginCollisionPlayer(b2Body* /* player */) {
    m_game->take_hit();
    return false;
}
