#include <box2d/box2d.h>
#include "game/GameManager.hpp"
#include "game/Player.hpp"
#include "game/MyVulkanEngine.hpp"
#include "PhysicsListener.hpp"
#include "Physics.hpp"

Physics::Physics(b2World* world, GameManager* game_manager, MyVulkanEngine* mve)
    : VEEventListener("PhysicsListener"), m_active(false), m_accumulator{0.0f}, m_world{ world }, m_game{game_manager}, m_vulkan_engine{mve} {}

Physics::~Physics() {}

void Physics::activate() {
    m_active = true;
}

void Physics::deactivate() {
    m_active = false;
}

void Physics::register_listener(PhysicsListener *listener) {
    m_listeners.push_back(listener);
}

void Physics::onFrameStarted(ve::veEvent event) {
    if (m_active) {
        float frame_time = event.dt;
        if (frame_time > max_frame_time)
            frame_time = max_frame_time;

        m_accumulator += frame_time;
        while (m_accumulator >= time_step) {
            // To make sure we get a correct interpolation, we save the
            // state of the physics simulation (i.e. the position) for
            // all physics listeners
            for (PhysicsListener* listener : m_listeners)
                listener->savePreviousState();

            m_world->Step(
                time_step,
                velocityIterations,
                positionIterations

            );

            // Update all listeners per *physics step*
            for (PhysicsListener* listener : m_listeners)
                listener->onPhysicsStep(time_step);
 
            m_accumulator -= time_step;
        }
    }
 
    if (m_game->m_game_state == EGameState::LOST_LIFE) {
        deactivate();

        // Reset all listeners
        for (PhysicsListener* listener : m_listeners)
            listener->reset();

        m_game->m_game_state = EGameState::RUNNING;
        activate();
    }    

    if (m_game->m_game_state == EGameState::LOST) {
        deactivate();

        if (m_game->reset_game) {
            // Delete VEScenenodes and bodies
            for (PhysicsListener* listener : m_listeners) {
                listener->deleteNode();
            }

            // Reset Player
            m_vulkan_engine->get_player()->reset();

            m_game->CURRENT_LEVEL_NUMBER = 1;
            m_game->DUCK_LIVES = 3;
            m_game->reset_game = false;

            m_vulkan_engine->draw_level();
            m_game->m_game_state = EGameState::RUNNING;

            activate();
        }
    }

    if (m_game->m_game_state == EGameState::LEVEL_COMPLETED) {
        m_vulkan_engine->push_sound(SOUND_LEVEL_COMPLETED);
        deactivate();
        
        // Delete VEScenenodes and bodies
        for (PhysicsListener* listener : m_listeners) {
            listener->deleteNode();
        }

        // Reset Player
        m_vulkan_engine->get_player()->reset();

        m_game->CURRENT_LEVEL_NUMBER++;
        m_vulkan_engine->draw_level();

        m_game->m_game_state = EGameState::RUNNING;
        activate();
    }


    if (m_game->m_game_state == EGameState::WON && m_game->reset_game) {
        deactivate();
    
        // Delete VEScenenodes and bodies
        for (PhysicsListener* listener : m_listeners) {
            listener->deleteNode();
        }

        // Reset Player
        m_vulkan_engine->get_player()->reset();

        m_game->CURRENT_LEVEL_NUMBER = 1;
        m_game->DUCK_LIVES = 3;
        m_game->reset_game = false;

        m_vulkan_engine->draw_level();
        m_game->m_game_state = EGameState::RUNNING;

        activate();
    }

    // Update all physics listeners per *frame step*, this will
    // interpolate the physics step and update the vulkan engine scene
    // nodes accordingly.
    if (m_active) {
        for (PhysicsListener* listener : m_listeners) {
            listener->onInterpolation(
                event.dt,
                m_accumulator / time_step
            );
        }
    }
}
