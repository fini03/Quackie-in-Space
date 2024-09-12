#include <VEInclude.h>
#include <box2d/b2_body.h>
#include <tmxlite/Types.hpp>
#include "MyVulkanEngine.hpp"
#include "VEEventListener.h"
#include "eventlisteners/Physics.hpp"
#include "eventlisteners/Keys.hpp"
#include "eventlisteners/GUI.hpp"
#include "eventlisteners/CollisionListener.hpp"
#include "blocks/TileManager.hpp"
#include "blocks/PlatformManager.hpp"
#include "blocks/SlideManager.hpp"
#include "blocks/ButtonManager.hpp"
#include "blocks/FragileBlockManager.hpp"
#include "blocks/BlockManager.hpp"
#include "traps/TrapManager.hpp"
#include "Player.hpp"
#include "GameManager.hpp"
#include "Floor.hpp"
#include "stream/Stream.hpp"
#include "stream/EventReceiver.hpp"

using namespace ve;

MyVulkanEngine::MyVulkanEngine(
    bool debug,
    bool stream_enabled
) : VEEngine(veRendererType::VE_RENDERER_TYPE_FORWARD, debug),
    m_stream_enabled{stream_enabled}, m_stream_listener{nullptr} {}

MyVulkanEngine::~MyVulkanEngine() {}

void MyVulkanEngine::registerEventListeners() {
    player = new Player(&world, cameraParent);
    game_manager = new GameManager(&world, player, this);
    
    m_world_listener = new Physics(&world, game_manager, this);
    m_world_listener_keys = new Keys(&world, this, game_manager);
    m_world_listener_gui = new GUI(&world, this, game_manager); 
    m_world_listener_collisions = new CollisionListener(&world, player, m_world_listener_keys);

    block_manager = new BlockManager(&world, player, this);
    trap_manager = new TrapManager(&world, game_manager);
    fragile_block_manager = new FragileBlockManager(&world);
    tile_manager = new TileManager(&world);
    platform_manager = new PlatformManager(&world);
    slide_manager = new SlideManager(&world);
    button_manager = new ButtonManager(&world, trap_manager);

    // Registering physics related listeners
    world.SetContactListener(m_world_listener_collisions);
    m_world_listener->register_listener(m_world_listener_keys);
    m_world_listener->register_listener(game_manager);
    m_world_listener->register_listener(block_manager);
    m_world_listener->register_listener(trap_manager);
    m_world_listener->register_listener(fragile_block_manager);
    m_world_listener->register_listener(tile_manager);
    m_world_listener->register_listener(platform_manager);
    m_world_listener->register_listener(slide_manager);
    m_world_listener->register_listener(button_manager);
    m_world_listener->register_listener(player);

    registerEventListener(m_world_listener, { veEvent::VE_EVENT_FRAME_STARTED });
    registerEventListener(m_world_listener_keys, { veEvent::VE_EVENT_KEYBOARD });
    registerEventListener(m_world_listener_gui, { veEvent::VE_EVENT_DRAW_OVERLAY });
    registerEventListener(block_manager, { veEvent::VE_EVENT_KEYBOARD });

    // Set up stream if we want it
    if (m_stream_enabled) {
        m_stream_listener = new Stream(this, game_manager);
        m_event_receiver = new EventReceiver(m_stream_listener->sender);
        m_event_receiver->start_listening();

        registerEventListener(m_stream_listener, {
            veEvent::VE_EVENT_FRAME_STARTED,
            veEvent::VE_EVENT_FRAME_ENDED,
            veEvent::VE_EVENT_KEYBOARD
        });
        registerEventListener(m_event_receiver, {
            veEvent::VE_EVENT_FRAME_STARTED
        });
    } else {
        m_stream_listener = nullptr;
        m_event_receiver = nullptr;
    }
}

void MyVulkanEngine::load_level() {			
    //camera parent is used for translations
	cameraParent = getSceneManagerPointer()->createSceneNode("StandardCameraParent", getRoot(), glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

	//camera can only do yaw (parent y-axis) and pitch (local x-axis) rotations
	VkExtent2D extent = getWindowPointer()->getExtent();
	VECameraProjective *camera = (VECameraProjective *)getSceneManagerPointer()->createCamera("StandardCamera", VECamera::VE_CAMERA_TYPE_PROJECTIVE, cameraParent);
	camera->m_nearPlane = 0.1f;
	camera->m_farPlane = 500.1f;
	camera->m_aspectRatio = extent.width / (float)extent.height;
	camera->m_fov = 45.0f;
	camera->lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	getSceneManagerPointer()->setCamera(camera);

    VELight *light4 = (VESpotLight *)getSceneManagerPointer()->createLight("StandardAmbientLight", VELight::VE_LIGHT_TYPE_AMBIENT, camera);
	light4->m_col_ambient = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);

	//use one light source
	VELight *light1 = (VEDirectionalLight *)getSceneManagerPointer()->createLight("StandardDirLight", VELight::VE_LIGHT_TYPE_DIRECTIONAL, getRoot());     //new VEDirectionalLight("StandardDirLight");
    light1->lookAt(glm::vec3(29.0f, 19.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f));
	light1->m_col_diffuse = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
	light1->m_col_specular = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f); 

    registerEventListeners();
}

void MyVulkanEngine::setup_game() {
    load_level();
    create_camera_and_scene();
    draw_level();

    if (m_stream_enabled)
        game_manager->m_game_state = EGameState::START;

    m_world_listener->activate();
}

void MyVulkanEngine::create_camera_and_scene() {

    VESceneNode *pScene;																	// Get Root Node
	VECHECKPOINTER( pScene =
	getSceneManagerPointer()->createSceneNode("Level 1", getRoot()) );
	
	// Scene models
    VESceneNode *sp1;
	VECHECKPOINTER( sp1 = getSceneManagerPointer()->createSkybox( "The Sky", "media/models/sky/cloudy",
                { "galaxy_ft.jpg", "galaxy_bk.jpg", "galaxy_up.jpg", "galaxy_dn.jpg",
			    "galaxy_rt.jpg", "galaxy_lf.jpg" }, pScene) );

    floor = new Floor(&world, game_manager);

	VEEntity *pE4;
	VECHECKPOINTER( pE4 = (VEEntity*)getSceneManagerPointer()->getSceneNode("The Plane/plane_t_n_s.obj/plane/Entity_0") );
	pE4->setParam( glm::vec4(1000.0f, 1000.0f, 0.0f, 0.0f) );
    getSceneManagerPointer()->getSceneNode("StandardCameraParent")->setPosition({0,3.5,-8});
}

void MyVulkanEngine::load_model(const int tile_ID, const glm::vec3& position, bool custom_prop = false) {
    switch(tile_ID) {
        case 17:
            tile_manager->create_tile(position);
            break;
        case 29: // Slide
            slide_manager->create_slide(position);
            break;
        case 56: // Saw
            trap_manager->create_saw(position, custom_prop);
            break;
        case 16: // Button
            button_manager->create_button(position);
            break;
        case 41: // Fire
            trap_manager->create_fire_block(position);
            break;
        case 11: // Box
            block_manager->create_block(position);
            break;
        case 10: // Laser
            trap_manager->create_laserbeam(position, custom_prop);
            break;
        case 14: // Fragile Blocks
            fragile_block_manager->create_fragile_block(position);
            break;
        case 25: // Platform for moving up 
            platform_manager->create_platform(position);
            break;
        case 27: // Key
            game_manager->create_key(position);
            break;
        case 34: // Goal
            game_manager->create_goal(position);
            break;
        default:
            break;
    }
}

void MyVulkanEngine::draw_level() {
    tmx::Map map = tmxparser.parse_map(PATH_TO_LEVELS + "lvl" + std::to_string(game_manager->CURRENT_LEVEL_NUMBER) + ".tmx");
    //tmx::Map map = tmxparser.parse_map(PATH_TO_LEVELS + "lvl1.tmx");
    
    // Iterating over each layer and drawing the obj file accordingly
    for(const auto& layer : map.getLayers()) {
        if(layer->getType() == tmx::Layer::Type::Tile) {
            const auto& tile_layer = static_cast<const tmx::TileLayer&>(*layer);
            const auto& tiles = tile_layer.getTiles();
            const int width = tile_layer.getSize().x;
            const int height = tile_layer.getSize().y;
            
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    const int tile_ID = tiles[(height - (y + 1)) * width + x].ID;
                    const glm::vec3 position(x, y + 0.5, 0);
                    load_model(tile_ID, position, false); 
                }
            }
        }
        if (layer->getType() == tmx::Layer::Type::Object) {
            const auto& object_layer = static_cast<const tmx::ObjectGroup&>(*layer);
            for (const auto& object : object_layer.getObjects()) {
                for(const auto& prop : object.getProperties()) {
                    if (prop.getType() == tmx::Property::Type::Boolean) {
                        const bool custom_prop = prop.getBoolValue();
                        const auto& position = object.getPosition() / tmx::Vector2f(map.getTileSize().x, map.getTileSize().y);
                        const glm::vec3 obj_position(position.x, 20.0 - position.y + 0.5f, 0);
                        const int tile_ID = object.getTileID();

                        load_model(tile_ID, obj_position, custom_prop); 
                    } 
                }
            }
        }
    }
}

Player* MyVulkanEngine::get_player() const {
    return player;
}

GameManager* MyVulkanEngine::get_game_manager() const {
    return game_manager;
}

PlatformManager* MyVulkanEngine::get_platform_manager() const {
    return platform_manager;
}

BlockManager* MyVulkanEngine::get_block_manager() const {
    return block_manager;
}

SlideManager* MyVulkanEngine::get_slide_manager() const {
    return slide_manager;
}

FragileBlockManager* MyVulkanEngine::get_fragile_block_manager() const {
    return fragile_block_manager;
}

TrapManager* MyVulkanEngine::get_trap_manager() const {
    return trap_manager;
}

ButtonManager* MyVulkanEngine::get_button_manager() const {
    return button_manager;
}

void MyVulkanEngine::push_sound(Sound sound) {
    if (m_stream_listener)
        m_stream_listener->push_sound(sound);
}

void MyVulkanEngine::push_music(Music music) {
    if (m_stream_listener)
        m_stream_listener->push_music(music);
}
