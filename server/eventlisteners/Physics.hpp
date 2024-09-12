#pragma once

#include <VEInclude.h>

class MyVulkanEngine;
class PhysicsListener;
class b2World;
class GameManager;
class MyVulkanEngine;

class Physics : public ve::VEEventListener {
    private:
        const int velocityIterations = 6;
        const int positionIterations = 2;
        const float max_frame_time = 0.25f;
        const float time_step = 1.0f / 60.0f;
        bool m_active;
        float m_accumulator;
        b2World* m_world;
        GameManager* m_game;
        MyVulkanEngine* m_vulkan_engine;
        std::vector<PhysicsListener*> m_listeners;

    protected:
        void onFrameStarted(ve::veEvent event);

    public:
        Physics(b2World* world, GameManager* game_manager, MyVulkanEngine* mve);
        virtual ~Physics();
        void activate();
        void deactivate();
        void register_listener(PhysicsListener* listener);
};
