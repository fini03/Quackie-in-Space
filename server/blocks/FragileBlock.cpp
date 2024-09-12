#include "FragileBlock.hpp"
#include "game/GameManager.hpp"
#include <VEInclude.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <glm/glm.hpp>

using namespace ve;

FragileBlock::FragileBlock(b2World* world, const glm::vec3& position) {
    STARTING_POSITION = b2Vec2{position.x, position.y};
    starting_position = position;
    b2BodyDef bodyDef{};
    bodyDef.type = b2_kinematicBody;

    b2PolygonShape dynamicBox{};
    dynamicBox.SetAsBox(0.35f, 0.35f);

    b2FixtureDef fixtureDef{};
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 0.0f;
    fixtureDef.friction = 0.8f;
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    
    VECHECKPOINTER(m_node = getSceneManagerPointer()->loadModel("Fragile Block" + std::to_string(position.x) + "-" + std::to_string(position.y), "media/models/Fragile Block", "cube.obj", 0, getRoot()));
            
    bodyDef.position.Set(position.x, position.y);

    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);

    // Create a scaling matrix
    glm::mat4 scaleMatrix = glm::scale(glm::vec3(0.7f, 0.7f, 0.7f));

    // Apply the scaling matrix to the object's transform
    m_node->setTransform(scaleMatrix);
    m_node->setPosition(position);
     
    m_body = world->CreateBody(&bodyDef);
    m_body->CreateFixture(&fixtureDef);

    b2PolygonShape polygonShape;
    polygonShape.SetAsBox(0.35, 0.35);
    fixtureDef.shape = &polygonShape;
    fixtureDef.isSensor = true;
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    m_body->CreateFixture(&fixtureDef);
}

void FragileBlock::respawn() {
    m_body->SetEnabled(true);
    m_body->SetAwake(true);
    m_last_post = starting_position;
    m_node->setPosition(starting_position);
    m_body->SetLinearVelocity(b2Vec2{0.0f, 0.0f});
    m_body->SetTransform(b2Vec2{starting_position.x, starting_position.y}, m_body->GetAngle());
    getSceneManagerPointer()->setVisibility(m_node, true);

    // Reset any necessary state
    is_triggered = false;
    falling_countdown = 0.3f;
    respawn_timer = 1.0f;
}

void FragileBlock::stop_movement() {
    m_body->SetLinearVelocity(b2Vec2{0.0f, 0.0f});
    
    getSceneManagerPointer()->setVisibility(m_node, false);
    m_body->SetEnabled(false);
}


void FragileBlock::move_fragile_block() {
    b2Vec2 position = m_body->GetPosition();
    
    if (position.y > starting_position.y - maximum_y_blocks_down) {
        m_body->SetLinearVelocity(b2Vec2{0.0f, -2.0f});
    } else {
        stop_movement();
    }
}

void FragileBlock::onPhysicsStep(float dt) {
    if(is_triggered)
        falling_countdown -= dt;

    if(falling_countdown <= 0 && is_triggered) {
        move_fragile_block();
        respawn_timer -= dt;
    }
    
    if(!m_body->IsEnabled() && respawn_timer <= 0.0f)
        respawn();
}

void FragileBlock::reset() {
    respawn();
}

FragileBlock::~FragileBlock() {
    getSceneManagerPointer()->deleteSceneNodeAndChildren(m_node->getName());
    b2World* world = m_body->GetWorld();
    world->DestroyBody( m_body );
}

bool FragileBlock::beginCollisionPlayer(b2Body* player) {
    is_triggered = true;
    return true;
}
