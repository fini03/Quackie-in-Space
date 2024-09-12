#include <VEInclude.h>
#include <box2d/box2d.h>
#include <GLFW/glfw3.h>
#include "NetworkDefines.hpp"
#include "game/GameManager.hpp"
#include "game/MyVulkanEngine.hpp"
#include "game/Player.hpp"
#include "Keys.hpp"

using namespace ve;

Keys::Keys(b2World* world, MyVulkanEngine* mve, GameManager* game_manager)
    : VEEventListener("Keys"), m_vulkan_engine{ mve }, m_world{ world }, m_game{game_manager} {}

Keys::~Keys() {}

void Keys::onPhysicsStep(float dt) {
    m_jump_timeout = m_jump_timeout - dt;

    if(m_jump_timeout < 0) m_jump_timeout = 0;
    move_player();
}

bool Keys::onKeyboard(veEvent event) {
    Player* player = m_vulkan_engine->get_player();
    VESceneNode* player_node = player->get_node();

    switch (event.idata1) {
        case GLFW_KEY_W: // Jump
            if(event.idata3 == GLFW_PRESS) {
                move_transition(W_PRESSED);
            }
            break;
        case GLFW_KEY_A: // Move left
            if (event.idata3 == GLFW_PRESS) {
                player_node->setTransform(glm::rotate(glm::translate(glm::mat4(1.0), player_node->getPosition()), glm::radians(-90.0f), glm::vec3(0, 1, 0)));
                move_transition(A_PRESSED);
            } else if (event.idata3 == GLFW_RELEASE) {
                move_transition(A_RELEASED);
            }
            break;
        case GLFW_KEY_D: // Move right
            if (event.idata3 == GLFW_PRESS) {
                player_node->setTransform(glm::rotate(glm::translate(glm::mat4(1.0), player_node->getPosition()), glm::radians(90.0f), glm::vec3(0, 1, 0)));
                move_transition(D_PRESSED);
            } else if (event.idata3 == GLFW_RELEASE) {
                move_transition(D_RELEASED);
            }
            break;
        case GLFW_KEY_ESCAPE:
            exit(0);
            break;
        case GLFW_KEY_R:
            if (event.idata3 == GLFW_PRESS)
                m_game->reset_game = true;
            //else if (event.idata3 == GLFW_RELEASE)
            //    m_game->reset_game = false;
            break;
    }  
    return false;
}

void Keys::move_transition(const ETransition transition) {
    Player* player = m_vulkan_engine->get_player();
    b2Body* body = player->get_body();
    VESceneNode* player_node = player->get_node();

    if (transition == HIT_GROUND && m_jump_timeout > 0) {
        return;
    }

    // Transition lookup table
    static const std::unordered_map<std::pair<EState, ETransition>, EState, PairHash> transition_table {
        {{EState::STAND, ETransition::A_PRESSED}, EState::LEFT}, // turn left
        {{EState::STAND, ETransition::D_PRESSED}, EState::RIGHT}, // turn right
        {{EState::STAND, ETransition::W_PRESSED}, EState::JUMP}, // apply jump force
        {{EState::STAND, ETransition::A_RELEASED}, EState::STAND},
        {{EState::STAND, ETransition::D_RELEASED}, EState::STAND},
        {{EState::STAND, ETransition::HIT_GROUND}, EState::STAND},

        {{EState::LEFT, ETransition::A_PRESSED}, EState::LEFT},
        {{EState::LEFT, ETransition::D_PRESSED}, EState::RIGHT}, // turn right
        {{EState::LEFT, ETransition::W_PRESSED}, EState::JUMP_LEFT}, // apply jump force
        {{EState::LEFT, ETransition::A_RELEASED}, EState::STAND}, // turn front
        {{EState::LEFT, ETransition::D_RELEASED}, EState::LEFT},
        {{EState::LEFT, ETransition::HIT_GROUND}, EState::LEFT},
        
        {{EState::RIGHT, ETransition::A_PRESSED}, EState::LEFT}, // turn left
        {{EState::RIGHT, ETransition::D_PRESSED}, EState::RIGHT},
        {{EState::RIGHT, ETransition::W_PRESSED}, EState::JUMP_RIGHT}, // apply jump force
        {{EState::RIGHT, ETransition::A_RELEASED}, EState::RIGHT},
        {{EState::RIGHT, ETransition::D_RELEASED}, EState::STAND}, // turn front
        {{EState::RIGHT, ETransition::HIT_GROUND}, EState::RIGHT},
        
        {{EState::JUMP, ETransition::A_PRESSED}, EState::JUMP_LEFT}, // turn left
        {{EState::JUMP, ETransition::D_PRESSED}, EState::JUMP_RIGHT}, // turn right
        {{EState::JUMP, ETransition::W_PRESSED}, EState::JUMP},
        {{EState::JUMP, ETransition::A_RELEASED}, EState::JUMP},
        {{EState::JUMP, ETransition::D_RELEASED}, EState::JUMP},
        {{EState::JUMP, ETransition::HIT_GROUND}, EState::STAND},
        
        {{EState::JUMP_LEFT, ETransition::A_PRESSED}, EState::JUMP_LEFT},
        {{EState::JUMP_LEFT, ETransition::D_PRESSED}, EState::JUMP_RIGHT}, // turn right
        {{EState::JUMP_LEFT, ETransition::W_PRESSED}, EState::JUMP_LEFT},
        {{EState::JUMP_LEFT, ETransition::A_RELEASED}, EState::JUMP}, // turn front
        {{EState::JUMP_LEFT, ETransition::D_RELEASED}, EState::JUMP_LEFT},
        {{EState::JUMP_LEFT, ETransition::HIT_GROUND}, EState::LEFT},

        {{EState::JUMP_RIGHT, ETransition::A_PRESSED}, EState::JUMP_LEFT}, // turn left
        {{EState::JUMP_RIGHT, ETransition::D_PRESSED}, EState::JUMP_RIGHT},
        {{EState::JUMP_RIGHT, ETransition::W_PRESSED}, EState::JUMP_RIGHT},
        {{EState::JUMP_RIGHT, ETransition::A_RELEASED}, EState::JUMP_RIGHT},
        {{EState::JUMP_RIGHT, ETransition::D_RELEASED}, EState::JUMP}, // turn front
        {{EState::JUMP_RIGHT, ETransition::HIT_GROUND}, EState::RIGHT},
    };


    EState new_state = transition_table.at({m_state, transition});
    
    if (m_state == EState::STAND && new_state == EState::JUMP) {
        apply_jump_force(body);
    }

    if (m_state == EState::RIGHT && new_state == EState::JUMP_RIGHT) {
        apply_jump_force(body);
    }
    
    if (m_state == EState::LEFT && new_state == EState::JUMP_LEFT) {
        apply_jump_force(body);
    }

    if (new_state == EState::STAND) {
        body->SetLinearVelocity(b2Vec2{0.0, body->GetLinearVelocity().y});
    }

    if (new_state == EState::STAND || new_state == EState::JUMP) {
        player_node->setTransform(glm::rotate(glm::translate(glm::mat4(1.0), player_node->getPosition()), glm::radians(180.0f), glm::vec3(0, 1, 0)));
    }

    m_state = new_state;
}

void Keys::move_player() {
    Player* player = m_vulkan_engine->get_player();
    b2Body* body = player->get_body();

    switch (m_state) {
        case EState::STAND:
            dampen_velocity(body);
            break;
        case EState::LEFT:
            apply_velocity(body, -MOVE_SPEED);
            break;
        case EState::RIGHT:
            apply_velocity(body, MOVE_SPEED);
            break;
        case EState::JUMP:
            dampen_velocity(body);
            break;
        case EState::JUMP_LEFT:
            apply_velocity(body, -MOVE_SPEED);
            break;
        case EState::JUMP_RIGHT:
            apply_velocity(body, MOVE_SPEED);
            break;
    }
}

void Keys::apply_jump_force(b2Body* body) {
    b2Vec2 velocity = body->GetLinearVelocity();
    float horizontalVelocity = velocity.x;
    float jumpDirection = horizontalVelocity * 0.1f;
    jumpDirection = std::clamp(jumpDirection, -0.2f, 0.2f);

    body->ApplyLinearImpulseToCenter(b2Vec2{0.0f, body->GetMass() * 5.1f}, true);
    m_jump_timeout = 0.1;
}

void Keys::apply_velocity(b2Body* body, float target_velocity) {
    b2Vec2 current_velocity = body->GetLinearVelocity();

    if (target_velocity < 0) {
        target_velocity = b2Max(current_velocity.x - 0.3f, target_velocity);
    } else {
        target_velocity = b2Min(current_velocity.x + 0.3f, target_velocity);
    }

    float velocity_change = target_velocity - current_velocity.x;
    float impulse = body->GetMass() * velocity_change;
    body->ApplyLinearImpulseToCenter(b2Vec2(impulse, 0), true);
    //body->SetLinearVelocity(b2Vec2{target_velocity, current_velocity.y});
}

void Keys::dampen_velocity(b2Body* body) {
    b2Vec2 current_velocity = body->GetLinearVelocity();
    float dampened_velocity = current_velocity.x * 0.95f;
    float velocity_change = dampened_velocity - current_velocity.x;
    float impulse = body->GetMass() * velocity_change;

    body->ApplyLinearImpulseToCenter(b2Vec2{impulse, 0}, true);
    //body->SetLinearVelocity(b2Vec2{0.0f, current_velocity.y});
}
