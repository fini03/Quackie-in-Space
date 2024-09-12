#include <VEInclude.h>
#include <box2d/box2d.h>
#include <glm/glm.hpp>
#include "Goal.hpp"
#include "GameManager.hpp"
#include "game/MyVulkanEngine.hpp"
#include "game/Player.hpp"
#include "Key.hpp"

using namespace ve;

Goal::Goal(b2World* world, const glm::vec3& position, GameManager* game_manager, Player* player, MyVulkanEngine* mve) : m_game{game_manager}, m_player{player}, m_vulkan_engine{mve} {
    b2BodyDef bodyDef{};
    bodyDef.type = b2_staticBody;
 
    VECHECKPOINTER(m_node = getSceneManagerPointer()->loadModel("Goal" + std::to_string(position.x) + "-" + std::to_string(position.y), "media/models/Goal", "goal.obj", 0, getRoot()));
            
    bodyDef.position.Set(position.x, position.y);

    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    m_node->setPosition(position);

    m_body = world->CreateBody(&bodyDef);

    b2FixtureDef fixtureDef{};
    b2PolygonShape polygonShape;
    polygonShape.SetAsBox(0.01, 0.01);
    fixtureDef.shape = &polygonShape;
    fixtureDef.isSensor = true;
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    m_body->CreateFixture(&fixtureDef);
}

Goal::~Goal() {
    getSceneManagerPointer()->deleteSceneNodeAndChildren(m_node->getName());
    b2World* world = m_body->GetWorld();
    world->DestroyBody( m_body );
}

bool Goal::beginCollisionPlayer(b2Body* player) {
    if(m_game->CURRENT_LEVEL_NUMBER >= 3) {
        m_game->m_game_state = EGameState::WON;
        m_vulkan_engine->push_sound(SOUND_GAME_WON);
        return false;
    }


    if(m_player->is_key_picked_up) {
        m_game->m_game_state = EGameState::LEVEL_COMPLETED;
    }

    return false;
}
