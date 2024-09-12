#pragma once

#include <VEInclude.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include "parser/TMXParser.hpp"
#include "NetworkDefines.hpp"

class GameManager;
class TileManager;
class PlatformManager;
class ButtonManager;
class BlockManager;
class FragileBlockManager;
class TrapManager;
class SlideManager;
class CollisionListener;
class Physics;
class Keys;
class Player;
class GUI;
class Floor;
class Stream;
class EventReceiver;

class MyVulkanEngine : public ve::VEEngine {
    private:
        Stream* m_stream_listener;
        EventReceiver* m_event_receiver;
        
        std::string PATH_TO_LEVELS = "media/levels/";
        TMXParser tmxparser;

        b2Vec2 gravity{0.0f, -10.0f};
        b2World world{gravity};

        GameManager* game_manager;
        TileManager* tile_manager;
        PlatformManager* platform_manager;
        ButtonManager* button_manager;
        BlockManager* block_manager;
        FragileBlockManager* fragile_block_manager;
        TrapManager* trap_manager;
        SlideManager* slide_manager;

        Physics* m_world_listener;
        Keys* m_world_listener_keys;
        CollisionListener* m_world_listener_collisions;
        GUI* m_world_listener_gui;

        ve::VESceneNode* cameraParent;
        Player* player;
        Floor* floor;
    public:
        MyVulkanEngine(
            bool debug = false,
            bool stream_enabled = false
        ); 
        ~MyVulkanEngine();
        
        void registerEventListeners() override;
        void load_level();
        void create_camera_and_scene();
        void setup_game();
        void load_model(const int tile_ID, const glm::vec3& position, bool move_down);
        void draw_level();

        Player* get_player() const;
        GameManager* get_game_manager() const;
        PlatformManager* get_platform_manager() const;
        BlockManager* get_block_manager() const;
        FragileBlockManager* get_fragile_block_manager() const;
        TrapManager* get_trap_manager() const;
        SlideManager* get_slide_manager() const;
        ButtonManager* get_button_manager() const;

        void push_sound(Sound sound);
        void push_music(Music sound);

        bool m_stream_enabled;
};

