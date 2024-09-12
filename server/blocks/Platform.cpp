#include "Platform.hpp"
#include "game/GameManager.hpp"
#include <VEInclude.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <glm/glm.hpp>

using namespace ve;

Platform::Platform(b2World* world, const glm::vec3& position) {
    STARTING_POSITION = b2Vec2{position.x, position.y};
    starting_position = position;
    b2BodyDef bodyDef{};
    bodyDef.type = b2_kinematicBody;

    b2PolygonShape dynamicBox{};
    dynamicBox.SetAsBox(0.45f, 0.45f);

    b2FixtureDef fixtureDef{};
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 1.0f;
    
    VECHECKPOINTER(m_node = getSceneManagerPointer()->loadModel("Platform" + std::to_string(position.x) + "-" + std::to_string(position.y), "media/models/Platform", "cube.obj", 0, getRoot()));
            
    bodyDef.position.Set(position.x, position.y);

    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    m_node->setPosition(position);
    
    // Create a scaling matrix
    glm::mat4 scaleMatrix = glm::scale(glm::vec3(0.90f, 0.90f, 0.90f));

    // Apply the scaling matrix to the object's transform
    m_node->setTransform(scaleMatrix);

    m_body = world->CreateBody(&bodyDef);
    m_body->CreateFixture(&dynamicBox, 0.0f);


    b2PolygonShape polygonShape;
    polygonShape.SetAsBox(0.3, 0.3, b2Vec2(0,0.5), 0);
    fixtureDef.shape = &polygonShape;
    fixtureDef.isSensor = true;
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    m_body->CreateFixture(&fixtureDef);
}

Platform::~Platform() {
    getSceneManagerPointer()->deleteSceneNodeAndChildren(m_node->getName());
    b2World* world = m_body->GetWorld();
    world->DestroyBody( m_body );
}

void Platform::apply_velocity() {
    float velocity_y = MOVE_DISTANCE / MOVE_DURATION;
    m_body->SetLinearVelocity(b2Vec2{0.0f, velocity_y});
}

void Platform::fall_down() {
    b2Vec2 position = m_body->GetPosition();
    if (position.y >= starting_position.y) {
        float velocity_y = MOVE_DISTANCE / MOVE_DURATION;
        m_body->SetLinearVelocity(b2Vec2{0.0f, -velocity_y});
    } else {
        stop_movement();
        current_distance = 0.0f;
    } 
}

void Platform::stop_movement() {
    m_body->SetLinearVelocity(b2Vec2{0.0f, 0.0f});
}

void Platform::move_platform(const float dt) {
    float distance_to_move = MOVE_DISTANCE / MOVE_DURATION * dt;

    if (is_platform_moving) {
        apply_velocity();
        current_distance += distance_to_move;

        if (current_distance >= MOVE_DISTANCE && is_platform_moving) {
            stop_movement();
            current_distance -= distance_to_move;
        } else if (current_distance <= 0.0f && !is_platform_moving) {
            stop_movement();
            current_distance = 0.0f;
        }

    } else {
        fall_down();
        current_distance -= distance_to_move;
    }
}

void Platform::onPhysicsStep(float dt) {
    move_platform(dt);
}

void Platform::reset() {
    m_body->SetLinearVelocity(b2Vec2{0.0f, 0.0f});
    m_body->SetTransform(STARTING_POSITION, m_body->GetAngle());
    current_distance = 0.0f;
    is_platform_moving = false;
    m_body->SetAwake(true);
}

bool Platform::beginCollisionPlayer(b2Body* player) {
    is_platform_moving = true;
    return true;
}

bool Platform::endCollisionPlayer(b2Body* player) {
    is_platform_moving = false;
    return true;
}
