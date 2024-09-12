#include "Fire.hpp"
#include "Trap.hpp"
#include "game/GameManager.hpp"
#include <VEInclude.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_world.h>

using namespace ve;

Fire::Fire(b2World* world, const glm::vec3& position, GameManager* game_manager) : Trap(world, position, game_manager) {
    STARTING_POSITION = b2Vec2{position.x, position.y};
    b2BodyDef bodyDef{};

    b2PolygonShape dynamicBox{};
    dynamicBox.SetAsBox(0.5f, 0.5f);

    b2FixtureDef fixtureDef{};
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 1.0f;
    
    VECHECKPOINTER(m_node = getSceneManagerPointer()->loadModel("Fire" + std::to_string(position.x) + "-" + std::to_string(position.y), "media/models/Fire", "fire.obj", 0, getRoot()));
            
    bodyDef.position.Set(position.x, position.y);

    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    m_node->setPosition(position);
    
    m_body = world->CreateBody(&bodyDef);
    m_body->CreateFixture(&dynamicBox, 0.0f);


    b2PolygonShape polygonShape;
    polygonShape.SetAsBox(0.5, 0.5);
    fixtureDef.shape = &polygonShape;
    fixtureDef.isSensor = true;
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    m_body->CreateFixture(&fixtureDef);
}

Fire::~Fire() {
    getSceneManagerPointer()->deleteSceneNodeAndChildren(m_node->getName());
    b2World* world = m_body->GetWorld();
    world->DestroyBody( m_body );
}

bool Fire::beginCollisionPlayer(b2Body* player) {
    m_game->take_hit();
    return false;
}

void Fire::savePreviousState() {}

void Fire::onInterpolation(float /* frame_dt */, float /* ratio */) {}

void Fire::reset() {
    m_body->SetTransform(STARTING_POSITION, m_body->GetAngle());
}
