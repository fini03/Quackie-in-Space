#include <VEInclude.h>
#include <box2d/box2d.h>
#include "Key.hpp"
#include "NetworkDefines.hpp"
#include "game/MyVulkanEngine.hpp"
#include "game/Player.hpp"

using namespace ve;

Key::Key(b2World* world, const glm::vec3& position, Player* player, MyVulkanEngine* mve) : m_player{ player }, m_vulkan_engine{mve} {
    STARTING_POSITION = b2Vec2{position.x, position.y};
    b2BodyDef bodyDef{};
    bodyDef.type = b2_staticBody;
 
    VECHECKPOINTER(m_node = getSceneManagerPointer()->loadModel("Key" + std::to_string(position.x) + "-" + std::to_string(position.y), "media/models/Key", "key.obj", 0, getRoot()));
            
    bodyDef.position.Set(position.x, position.y);
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);

    m_node->setPosition(position);    
    m_body = world->CreateBody(&bodyDef);

    b2FixtureDef fixtureDef{};
    b2PolygonShape polygonShape;
    polygonShape.SetAsBox(0.1, 0.1, b2Vec2(0,0.2), 0);
    fixtureDef.shape = &polygonShape;

    fixtureDef.isSensor = true;
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    m_body->CreateFixture(&fixtureDef);
}

Key::~Key() {
    getSceneManagerPointer()->deleteSceneNodeAndChildren(m_node->getName());
    b2World* world = m_body->GetWorld();
    world->DestroyBody( m_body );
}

void Key::onPhysicsStep() {
    if (m_player->is_key_picked_up) {
        getSceneManagerPointer()->setVisibility(m_node, false);
        m_body->SetEnabled(false); 
    }
}

bool Key::beginCollisionPlayer(b2Body* player) {
    m_vulkan_engine->push_sound(SOUND_PICKUP_KEY);
    m_player->is_key_picked_up = true;
    return false;
}

void Key::reset() {
    m_body->SetEnabled(true);
    m_body->SetAwake(true);
    m_body->SetTransform(STARTING_POSITION, m_body->GetAngle());
    getSceneManagerPointer()->setVisibility(m_node, true);    
}
