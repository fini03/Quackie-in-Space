#include "Slide.hpp"
#include "game/GameManager.hpp"
#include <VEInclude.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <glm/glm.hpp>

using namespace ve;

Slide::Slide(b2World* world, const glm::vec3& position) {
    STARTING_POSITION = b2Vec2{position.x, position.y};
    starting_position = position;
    b2BodyDef bodyDef{};
    bodyDef.type = b2_kinematicBody;

    b2PolygonShape dynamicBox{};
    dynamicBox.SetAsBox(0.5f, 0.25f);

    b2FixtureDef fixtureDef{};
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 10.0f;
    fixtureDef.friction = 1.0f;
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    
    // Create scene node and attach to body
    VECHECKPOINTER(m_node = getSceneManagerPointer()->loadModel("Slide" + std::to_string(position.x) + "-" + std::to_string(position.y), "media/models/Slide", "cube.obj", 0, getRoot()));

    bodyDef.position.Set(position.x, position.y);
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    
    // Create a scaling matrix
    glm::mat4 scaleMatrix = glm::scale(glm::vec3(1.f, 0.50f, 1.f));

    // Apply the scaling matrix to the object's transform
    m_node->setTransform(scaleMatrix);
    m_node->setPosition(position);

    m_body = world->CreateBody(&bodyDef);
    m_body->CreateFixture(&fixtureDef);

    b2PolygonShape polygonShape;
    polygonShape.SetAsBox(0.45, 0.3);
    fixtureDef.shape = &polygonShape;
    fixtureDef.isSensor = true;
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    m_body->CreateFixture(&fixtureDef);
}

Slide::~Slide() {
    getSceneManagerPointer()->deleteSceneNodeAndChildren(m_node->getName());
    b2World* world = m_body->GetWorld();
    world->DestroyBody( m_body );
}

void Slide::move_slide(const float dt) {
    float velocity_x = MOVE_DISTANCE / MOVE_DURATION;
    float distance_to_move = MOVE_DISTANCE / MOVE_DURATION * dt;

    if (is_slide_moving) {
        m_body->SetLinearVelocity(b2Vec2{velocity_x, 0.0f});
        current_distance += distance_to_move; 

        if (current_distance >= MOVE_DISTANCE && is_slide_moving) {
            m_body->SetLinearVelocity(b2Vec2{0.0f, 0.0f});
            current_distance -= distance_to_move;
        } else if (current_distance <= 0.0f && !is_slide_moving) {
            m_body->SetLinearVelocity(b2Vec2{0.0f, 0.0f});
            current_distance = 0.0f;
        }
    } else {
        move_slide_back(velocity_x);
        current_distance -= distance_to_move;
    }
}

void Slide::move_slide_back(float velocity_x) {
    b2Vec2 position = m_body->GetPosition();
    if (position.x >= starting_position.x) {
        m_body->SetLinearVelocity(b2Vec2{-velocity_x, 0.0f});
    } else {
        m_body->SetLinearVelocity(b2Vec2{0.0f, 0.0f});
        current_distance = 0.0f;
    } 
}

void Slide::onPhysicsStep(float dt) {
    move_slide(dt);
}

void Slide::reset() {
    m_body->SetLinearVelocity(b2Vec2{0.0f, 0.0f});
    m_body->SetTransform(STARTING_POSITION, m_body->GetAngle());
    current_distance = 0.0f;
    is_slide_moving = false;
    m_body->SetAwake(true);
}

bool Slide::beginCollisionPlayer(b2Body* player) {
    is_slide_moving = true;
    return true;
}

bool Slide::endCollisionPlayer(b2Body* player) {
    is_slide_moving = false;
    return true;
}
