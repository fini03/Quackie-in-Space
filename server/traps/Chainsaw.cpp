#include "Chainsaw.hpp"
#include "Trap.hpp"
#include "game/GameManager.hpp"
#include <VEInclude.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

using namespace ve;

Chainsaw::Chainsaw(b2World* world, const glm::vec3& position, GameManager* game_manager, bool move_down) : Trap{world, position, game_manager}, move_down{move_down}, starting_position{position} {
    STARTING_POSITION = b2Vec2{position.x, position.y};
    b2BodyDef bodyDef{};
    bodyDef.type = b2_kinematicBody;

    Collider *collider = static_cast<Collider*>(this);

    b2PolygonShape dynamicBox{};
    dynamicBox.SetAsBox(0.5f, 0.5f);

    b2FixtureDef fixtureDef{};
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 1.0f;
    
    VECHECKPOINTER(m_node = getSceneManagerPointer()->loadModel("Sawies" + std::to_string(position.x) + "-" + std::to_string(position.y), "media/models/Saw", "sawies.obj", 0, getRoot()));
            
    bodyDef.position.Set(position.x, position.y);
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(collider);
    m_node->setPosition(position);
    
    m_body = world->CreateBody(&bodyDef);
    m_body->CreateFixture(&dynamicBox, 0.0f);

    b2PolygonShape polygonShape;
    polygonShape.SetAsBox(0.5, 0.5);
    fixtureDef.shape = &polygonShape;
    fixtureDef.isSensor = true;
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(collider);
    m_body->CreateFixture(&fixtureDef);
}

void Chainsaw::move_saw() {
    b2Vec2 position = m_body->GetPosition();
    float epsilon = 0.01f;  // Small threshold to handle floating-point precision issues

    if (move_down) {
        // Moving down first
        if (position.y > starting_position.y - maximum_y_blocks_down && position.y > starting_position.y - epsilon) {
            // Continue moving down
            m_body->SetLinearVelocity(b2Vec2{0.0f, -speed});
        } else if (position.y <= starting_position.y - maximum_y_blocks_down + epsilon) {
            // Reached the maximum downward distance, start moving back to the starting position
            m_body->SetLinearVelocity(b2Vec2{0.0f, speed});
        } else if (position.y >= starting_position.y - epsilon) {
            // Reached the starting position, stop
            m_body->SetLinearVelocity(b2Vec2{0.0f, 0.0f});
        }
    } else {
        // Moving up first
        if (position.y < starting_position.y + maximum_y_blocks_down && position.y < starting_position.y + epsilon) {
            // Continue moving up
            m_body->SetLinearVelocity(b2Vec2{0.0f, speed});
        } else if (position.y >= starting_position.y + maximum_y_blocks_down - epsilon) {
            // Reached the maximum upward distance, start moving back to the starting position
            m_body->SetLinearVelocity(b2Vec2{0.0f, -speed});
        } else if (position.y <= starting_position.y + epsilon) {
            // Reached the starting position, stop
            m_body->SetLinearVelocity(b2Vec2{0.0f, 0.0f});
        }
    }
}

void Chainsaw::onPhysicsStep(float /* dt */) {
    move_saw();
}

void Chainsaw::onInterpolation(float frame_dt, float ratio) {
    Trap::onInterpolation(frame_dt, ratio);

    m_node->setTransform(
        glm::rotate(m_node->getTransform(),
        10.0f * frame_dt,
        glm::vec3(0, 0, 1))
    );
}

Chainsaw::~Chainsaw() {
    getSceneManagerPointer()->deleteSceneNodeAndChildren(m_node->getName());
    b2World* world = m_body->GetWorld();
    world->DestroyBody( m_body );
}

bool Chainsaw::beginCollisionPlayer(b2Body* player) {
    m_game->take_hit();
    return false;
}

void Chainsaw::reset() {
    m_body->SetTransform(STARTING_POSITION, m_body->GetAngle()); 
}
