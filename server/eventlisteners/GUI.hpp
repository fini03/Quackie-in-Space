#include <VEInclude.h>

class b2World;
class MyVulkanEngine;
class GameManager;
class ESubrender_Nuklear;
class Player;

struct gui_font {
   struct nk_font *font;
};

class GUI : public ve::VEEventListener {
private:
    b2World* m_world;
    MyVulkanEngine* m_vulkan_engine;
    GameManager* m_game;
    Player* m_player;

    struct nk_image heart_icon;
    struct nk_image key_icon;
    struct nk_image box_icon;
    struct nk_image duck_butt;
    struct nk_image background_image;

    struct gui_font fonts[7];
    struct nk_font* droid;
    struct nk_font* roboto;

    ve::VESubrender_Nuklear* pSubrender;
    nk_context* ctx;

    void drawOverlayWindow(const char*, const char*, const char*, const char*);
    void drawMainMenu(const char*, const char*, const char*, const char*);
    void drawLivesBar();
    void drawInventory();
    void loadFont();

    struct nk_image loadBackground();
    struct nk_image loadHeartIcon();
    struct nk_image loadKeyIcon();
    struct nk_image loadBoxIcon();
    struct nk_image loadDuckButt();
protected:
    void onDrawOverlay(ve::veEvent event) override;
public:
    GUI(b2World* world, MyVulkanEngine* mve, GameManager* game_manager);
    virtual ~GUI();
};
