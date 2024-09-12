#include <box2d/box2d.h>
#include <VEInclude.h>
#include "Block.hpp"
#include "VESceneManager.h"
#include "blocks/BlockManager.hpp"
#include "game/MyVulkanEngine.hpp"
#include "game/Player.hpp"

using namespace ve;

Block::Block(b2World* world, const glm::vec3& position, BlockManager* manager, Player* player, MyVulkanEngine* mve) : m_manager{manager}, m_player{player}, m_vulkan_engine{mve} {
    STARTING_POSITION = b2Vec2{position.x, position.y};
    starting_position = position;
    b2BodyDef bodyDef{};
    bodyDef.type = b2_dynamicBody;
    
    // Create scene node and attach to body
    VECHECKPOINTER(m_node = getSceneManagerPointer()->loadModel("Box" + std::to_string(position.x) + "-" + std::to_string(position.y), "media/models/Block", "cube.obj", 0, getRoot()));
    bodyDef.position.Set(position.x, position.y);
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    
    // Create a scaling matrix
    glm::mat4 scaleMatrix = glm::scale(glm::vec3(0.70f, 0.70f, 0.70f));

    // Apply the scaling matrix to the object's transform
    m_node->setTransform(scaleMatrix);
    m_node->setPosition(position);

    // Create body and fixture
    m_body = world->CreateBody(&bodyDef);
    m_body->SetFixedRotation(true);
    
    // Create capsule-like collider
    float radius = 0.25f;
    
    b2PolygonShape shape;
    
    // Create fixture definition
    b2FixtureDef fixtureDef;
    fixtureDef.density = 100.0f;
    fixtureDef.friction = 0.2f;
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
     
    // Create bottom circle
    b2CircleShape circleBottom;
    circleBottom.m_radius = radius;
    circleBottom.m_p.Set(0.0f, -0.1);
    
    fixtureDef.shape = &circleBottom;
    m_body->CreateFixture(&fixtureDef);
    
    // Create rectangular shape connecting circles
    b2PolygonShape rectShape;
    rectShape.SetAsBox(0.35, 0.33);
    
    fixtureDef.shape = &rectShape;
    m_body->CreateFixture(&fixtureDef); 

    // Set linear damping
    m_body->SetLinearDamping(20.0f);

    // TODO: This is used to counteract linear damping in gravity 
    // direction and therefore a kinda ugly hack. Would be better to add
    // custom linear damping
    m_body->SetGravityScale(10.0f);

    b2FixtureDef sensorDef{};
    sensorDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    b2PolygonShape polygonShape;
    polygonShape.SetAsBox(0.4, 0.4);
    sensorDef.shape = &polygonShape;
    sensorDef.isSensor = true;
    m_body->CreateFixture(&sensorDef);
}

void Block::onInterpolation(float frame_dt, float ratio) {
    //std::cout << m_manager->is_E_key_pressed << " " << is_colliding << " " << m_manager->is_block_picked_up << std::endl;
    if(m_manager->is_E_key_pressed && is_colliding && m_player->picked_up_block == nullptr) {
        pickup();
    } else if(m_manager->is_space_key_pressed && !is_colliding && m_player->picked_up_block == this) {
        b2Body* player_body = m_player->get_body();
        drop(glm::vec3{player_body->GetPosition().x + 1.0f, player_body->GetPosition().y, 0.0f});
    }
    InterpolatedObject::onInterpolation(frame_dt, ratio);
}

void Block::reset() {
    m_body->SetTransform(STARTING_POSITION, m_body->GetAngle());
    m_body->SetEnabled(true);

    // After setting transform manually, we need to wake up the body
    m_body->SetAwake(true);
    
    getSceneManagerPointer()->setVisibility(m_node, true);

    is_colliding = false;
}

bool Block::beginCollisionPlayer(b2Body* player) {
    is_colliding = true;
    return true;
}

bool Block::endCollisionPlayer(b2Body* /* player */) {
    is_colliding = false;
    return true;
}

void Block::pickup() {
    m_vulkan_engine->push_sound(SOUND_PICKUP_BLOCK);
    // Set the visibility of the scene node to false
    getSceneManagerPointer()->setVisibility(m_node, false);
    m_body->SetEnabled(false);

    m_player->picked_up_block = this;
}

void Block::drop(const glm::vec3& new_position) {
    m_vulkan_engine->push_sound(SOUND_DROP_BLOCK);
    // Set the visibility of the scene node to true
    getSceneManagerPointer()->setVisibility(m_node, true);
    m_body->SetEnabled(true);

    m_player->picked_up_block = nullptr;

    // Set the new position of the block
    m_node->setPosition(new_position);
    m_body->SetTransform(b2Vec2(new_position.x, new_position.y), m_body->GetAngle());
    m_last_post = new_position;
}

Block::~Block() {
    getSceneManagerPointer()->deleteSceneNodeAndChildren(m_node->getName());
    b2World* world = m_body->GetWorld();
    world->DestroyBody( m_body );
}
