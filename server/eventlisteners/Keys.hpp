#pragma once

#include <VEInclude.h>
#include "PhysicsListener.hpp"

class MyVulkanEngine;
class b2World;
class b2Body;
class GameManager;

enum EState { STAND, LEFT, RIGHT, JUMP, JUMP_LEFT, JUMP_RIGHT };
enum ETransition { A_PRESSED, D_PRESSED, W_PRESSED, A_RELEASED, D_RELEASED, HIT_GROUND };

// Hash function for EState enum class
struct EStateHash {
    std::size_t operator()(EState const& state) const noexcept {
        return static_cast<std::size_t>(state);
    }
};

// Hash function for ETransition enum class
struct ETransitionHash {
    std::size_t operator()(ETransition const& transition) const noexcept {
        return static_cast<std::size_t>(transition);
    }
};

// Hash function for std::pair<EState, ETransition>
struct PairHash {
    std::size_t operator()(std::pair<EState, ETransition> const& pair) const noexcept {
        return std::hash<int>{}(static_cast<int>(pair.first)) ^ std::hash<int>{}(static_cast<int>(pair.second));
    }
};

class Keys : public ve::VEEventListener, public PhysicsListener {
    private:
        MyVulkanEngine* m_vulkan_engine;
        GameManager* m_game;
        b2World* m_world; // Pointer to the physics world
        EState m_state = EState::STAND;
        ETransition m_transition = ETransition::HIT_GROUND;

        const float MOVE_SPEED = 4.0f;
        float m_jump_timeout = 0;

        void move_player();
        void apply_jump_force(b2Body* body);
        void apply_velocity(b2Body* body, float target_velocity);
        void dampen_velocity(b2Body* body);
    protected:
        bool onKeyboard(ve::veEvent event);
    public:
        Keys(b2World* world, MyVulkanEngine* mve, GameManager* game_manager);
        virtual ~Keys();

        void move_transition(const ETransition transition);
        void onPhysicsStep(float dt);
};

