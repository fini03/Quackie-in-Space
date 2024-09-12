#pragma once

#include <box2d/b2_world.h>

class MyVulkanEngine;
class Keys;
class Player;

class CollisionListener : public b2ContactListener {
    private:
        MyVulkanEngine* m_vulkan_engine;
        Keys* m_physics_keys_listener;
        Player* m_player;

    protected:
        b2World* m_world; // Pointer to the physics world

	public:
		CollisionListener(b2World* world, Player* player, Keys* physics_keys_listener);
		virtual ~CollisionListener();
        
        void BeginContact(b2Contact* contact);
        void EndContact(b2Contact* contact);
};
