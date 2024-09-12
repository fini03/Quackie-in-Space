#include "eventlisteners/Physics.hpp"
#include "stb_image.h"
#include <VEInclude.h>
#include <box2d/box2d.h>
#include "GUI.hpp"
#include "game/GameManager.hpp"
#include "game/Player.hpp"
#include "game/MyVulkanEngine.hpp"

GUI::GUI(b2World* world, MyVulkanEngine* mve, GameManager* game_manager) 
    : VEEventListener("GUI"), m_world{ world }, m_vulkan_engine{ mve }, m_game{game_manager} {


	pSubrender = (ve::VESubrender_Nuklear*)m_vulkan_engine->getRenderer()->getOverlay();
    ctx = pSubrender->getContext();
    heart_icon = loadHeartIcon();
    key_icon = loadKeyIcon();
    box_icon = loadBoxIcon();
    duck_butt = loadDuckButt();
    background_image = loadBackground();
    
    loadFont();
    
    m_player = m_vulkan_engine->get_player();
}

void GUI::loadFont() {
    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    for (int i = 0 ; i < 7; i++) {
       fonts[i].font = nk_font_atlas_add_from_file(atlas, "media/fonts/DroidSans.ttf", (i + 1) * 6, 0);
    }

    nk_glfw3_font_stash_end();
    nk_style_set_font(ctx, &fonts[0].font->handle);
}

GUI::~GUI() {}

struct nk_image GUI::loadBackground() {
    ve::VETexture* texture = m_vulkan_engine->getSceneManager()->createTexture("menu", "media/images/", "menu.png");

    if (!texture) {
        std::cerr << "Texture loading failed" << std::endl;
        exit(1);
        // Handle error loading image
        struct nk_image img = {};
        img.handle.ptr = nullptr;
        img.handle.id = 0;
        img.w = 0;
        img.h = 0;
        return img;
    }
 
    void* texture_ptr = pSubrender->addTexture(texture);

    // Create nk_image
    struct nk_image img;
    img.handle.ptr = texture_ptr;
    img.w = texture->m_extent.width;
    img.h = texture->m_extent.height;
     
    return img;
}

struct nk_image GUI::loadHeartIcon() {
    ve::VETexture* texture = m_vulkan_engine->getSceneManager()->createTexture("heart", "media/images/", "heart.png");

    if (!texture) {
        std::cerr << "Texture loading failed" << std::endl;
        exit(1);
        // Handle error loading image
        struct nk_image img = {};
        img.handle.ptr = nullptr;
        img.handle.id = 0;
        img.w = 0;
        img.h = 0;
        return img;
    }
 
    void* texture_ptr = pSubrender->addTexture(texture);

    // Create nk_image
    struct nk_image img;
    img.handle.ptr = texture_ptr;
    img.w = texture->m_extent.width;
    img.h = texture->m_extent.height;
     
    return img;
}

struct nk_image GUI::loadKeyIcon() {
    ve::VETexture* texture = m_vulkan_engine->getSceneManager()->createTexture("key", "media/images/", "key.png");

    if (!texture) {
        std::cerr << "Texture loading failed" << std::endl;
        exit(1);
        // Handle error loading image
        struct nk_image img = {};
        img.handle.ptr = nullptr;
        img.handle.id = 0;
        img.w = 0;
        img.h = 0;
        return img;
    }
 
    void* texture_ptr = pSubrender->addTexture(texture);

    // Create nk_image
    struct nk_image img;
    img.handle.ptr = texture_ptr;
    img.w = texture->m_extent.width;
    img.h = texture->m_extent.height;
     
    return img;
}

struct nk_image GUI::loadBoxIcon() {
    ve::VETexture* texture = m_vulkan_engine->getSceneManager()->createTexture("block", "media/images/", "block.png");

    if (!texture) {
        std::cerr << "Texture loading failed" << std::endl;
        exit(1);
        // Handle error loading image
        struct nk_image img = {};
        img.handle.ptr = nullptr;
        img.handle.id = 0;
        img.w = 0;
        img.h = 0;
        return img;
    }
 
    void* texture_ptr = pSubrender->addTexture(texture);

    // Create nk_image
    struct nk_image img;
    img.handle.ptr = texture_ptr;
    img.w = texture->m_extent.width;
    img.h = texture->m_extent.height;
     
    return img;
}

struct nk_image GUI::loadDuckButt() {
    ve::VETexture* texture = m_vulkan_engine->getSceneManager()->createTexture("duck", "media/images/", "duck.png");

    if (!texture) {
        std::cerr << "Texture loading failed" << std::endl;
        exit(1);
        // Handle error loading image
        struct nk_image img = {};
        img.handle.ptr = nullptr;
        img.handle.id = 0;
        img.w = 0;
        img.h = 0;
        return img;
    }
 
    void* texture_ptr = pSubrender->addTexture(texture);

    // Create nk_image
    struct nk_image img;
    img.handle.ptr = texture_ptr;
    img.w = texture->m_extent.width;
    img.h = texture->m_extent.height;
    return img;
}


void GUI::drawLivesBar() {
    // Transparent background
    nk_color background = nk_rgba(0, 0, 0, 0);
    nk_style_push_color(ctx, &ctx->style.window.fixed_background.data.color, background);

    // Get window dimensions
    GLFWwindow* glfw_window = ((ve::VEWindowGLFW *)ve::getWindowPointer())->getWindowHandle();
    float xscale, yscale;
    if (glfw_window == nullptr) {
        xscale = 1.0f;
        yscale = 1.0f;
    } else {
        glfwGetWindowContentScale(glfw_window, &xscale, &yscale);
    }
    float window_width = m_vulkan_engine->getWindow()->getExtent().width / xscale;
    float window_height = m_vulkan_engine->getWindow()->getExtent().height / yscale;

    // Calculate position and size as a percentage of window dimensions
    float lives_bar_height = window_height * 0.1f; // 10% of window height
    float lives_bar_width = lives_bar_height /* * ratio (it's 1) */ * 3;

    float lives_bar_x = window_width * 0.01f; // 1% from the left edge
    float lives_bar_y = window_height * 0.01f; // 1% from the top edge

    if (nk_begin(ctx, "Lives Bar", nk_rect(lives_bar_x, lives_bar_y, lives_bar_width, lives_bar_height), NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND)) {
        nk_layout_row_dynamic(ctx, lives_bar_height * 0.8f, 3); // Adjust row height based on the bar height

        for (int i = 0; i < m_game->DUCK_LIVES; ++i) {
            nk_image(ctx, heart_icon);
        }
    }

    nk_end(ctx);
    nk_style_pop_color(ctx);
}

void GUI::drawInventory() {
    // Transparent background
    nk_color background = nk_rgba(0, 0, 0, 0);
    nk_style_push_color(ctx, &ctx->style.window.fixed_background.data.color, background);
    
    // Get window dimensions
    GLFWwindow* glfw_window = ((ve::VEWindowGLFW *)ve::getWindowPointer())->getWindowHandle();
    float xscale, yscale;
    if (glfw_window == nullptr) {
        xscale = 1.0f;
        yscale = 1.0f;
    } else {
        glfwGetWindowContentScale(glfw_window, &xscale, &yscale);
    }

    float window_width = m_vulkan_engine->getWindow()->getExtent().width / xscale;
    float window_height = m_vulkan_engine->getWindow()->getExtent().height / yscale;

    // Calculate position and size as a percentage of window dimensions
    float inventory_height = window_height * 0.1f; // 10% of window height
    float inventory_width = inventory_height /* * ratio (it's 1) */ * 2;
    
    float inventory_x = window_width - inventory_width - window_width * 0.01f; // 1% from the right edge
    float inventory_y = window_height * 0.01f; // 1% from the top edge

    if (nk_begin(ctx, "Inventory", nk_rect(inventory_x, inventory_y, inventory_width, inventory_height), NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND)) {
        nk_layout_row_dynamic(ctx, inventory_height * 0.8, 2);

        if (m_player->is_key_picked_up) 
            nk_image(ctx, key_icon);

        if (m_player->picked_up_block != nullptr)
            nk_image(ctx, box_icon);
    }

    nk_end(ctx);
    nk_style_pop_color(ctx);
}

void GUI::drawOverlayWindow(const char* title, const char* label, const char* button1Label, const char* button2Label) {
    nk_style_set_font(ctx, &fonts[4].font->handle);

    ctx->style.text.color = nk_rgb(255, 255, 255);
    // Get the current window size
    GLFWwindow* glfw_window = ((ve::VEWindowGLFW *)ve::getWindowPointer())->getWindowHandle();
    float xscale, yscale;
    if (glfw_window == nullptr) {
        xscale = 1.0f;
        yscale = 1.0f;
    } else {
        glfwGetWindowContentScale(glfw_window, &xscale, &yscale);
    }
    float window_width = m_vulkan_engine->getWindow()->getExtent().width / xscale;
    float window_height = m_vulkan_engine->getWindow()->getExtent().height / yscale;

    nk_color background = nk_rgba(40, 40, 40, 200); // Dark grey with some transparency
    nk_style_push_color(ctx, &ctx->style.window.fixed_background.data.color, background);

    // Full-screen overlay window
    if (nk_begin(ctx, title, nk_rect(0, 0, window_width, window_height), NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND)) {
        // Push window style with transparency and padding
        struct nk_style* s = &ctx->style;
        nk_style_push_vec2(ctx, &s->window.padding, nk_vec2(20, 20));
        nk_style_push_float(ctx, &s->window.border, 2.0f);
        nk_style_push_color(ctx, &s->window.border_color, nk_rgba(60, 60, 60, 255));
        nk_style_push_color(ctx, &s->window.fixed_background.data.color, background);

        //// Center the content within the full-screen window 
        //float content_height = window_height * 0.4;
        //float content_width = content_height * (340.0f / 436.0f) * 2; // duck aspect ratio

        float group_height = window_height * 0.6;
        float group_width = group_height;
        float x = (window_width - group_width) / 2;
        float y = (window_height - group_height) / 2;

        nk_layout_space_begin(ctx, NK_STATIC, window_height, 1);
        nk_layout_space_push(ctx, nk_rect(x, y, group_width, group_height));


        //float image_height = group_height * (340 / 436.0f);

        float heading_height = group_height * 0.2;
        float spacing_height = group_height * 0.1;
        float button_height = group_height * 0.1;

        float image_height = group_height * 0.2;

        float image_width = image_height * (340.0f / 436.0f);
        float image_width_ratio = image_width / group_width;
        float image_row_leftover = (1.0f - image_width_ratio) / 2.0f;
        
        if (nk_group_begin(ctx, "Group", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
            // Show label
            nk_layout_row_dynamic(ctx, heading_height, 1);
            nk_label(ctx, label, NK_TEXT_CENTERED);
 
            // Spacing between the label row and the buttons
            nk_layout_row_dynamic(ctx, spacing_height, 1);
            
            // Set font
            nk_style_set_font(ctx, &fonts[2].font->handle);

            // Buttons
            nk_layout_row_dynamic(ctx, button_height, 1);
            nk_label(ctx, button1Label, NK_TEXT_CENTERED);
        
            nk_layout_row_dynamic(ctx, button_height, 1);
            nk_label(ctx, button2Label, NK_TEXT_CENTERED);

            // Spacing between the buttons and logo
            nk_layout_row_dynamic(ctx, spacing_height, 1);

            // Begin the layout row for the image
            nk_layout_row_begin(ctx, NK_DYNAMIC, image_height, 3);

            // Space left
            nk_layout_row_push(ctx, image_row_leftover);
            nk_spacer(ctx);

            // Image centered
            nk_layout_row_push(ctx, image_width_ratio);
            nk_image(ctx, duck_butt);

            // Space right
            nk_layout_row_push(ctx, image_row_leftover);
            nk_spacer(ctx);
            nk_layout_row_end(ctx);

            //nk_layout_row_end(ctx);
            nk_group_end(ctx);
        }

        nk_layout_space_end(ctx);

        // Pop style properties
        nk_style_pop_color(ctx);
        nk_style_pop_color(ctx);
        nk_style_pop_float(ctx);
        nk_style_pop_vec2(ctx);
    }
    nk_end(ctx);

    nk_style_pop_color(ctx);
}

void GUI::drawMainMenu(const char* title, const char* label, const char* button2Label, const char* button3Label) {
	//nk_style_push_font(ctx, &droid->handle);
    nk_style_set_font(ctx, &fonts[4].font->handle);
    ctx->style.text.color = nk_rgb(255, 255, 255);
    // Get the current window size
    GLFWwindow* glfw_window = ((ve::VEWindowGLFW *)ve::getWindowPointer())->getWindowHandle();
    float xscale, yscale;
    if (glfw_window == nullptr) {
        xscale = 1.0f;
        yscale = 1.0f;
    } else {
        glfwGetWindowContentScale(glfw_window, &xscale, &yscale);
    }
    float window_width = m_vulkan_engine->getWindow()->getExtent().width / xscale;
    float window_height = m_vulkan_engine->getWindow()->getExtent().height / yscale;

    nk_style_item background = nk_style_item_image(background_image);
    nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, background);

    // Full-screen main menu window
    if (nk_begin(ctx, title, nk_rect(0, 0, window_width, window_height), NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND)) {
        // Push window style with padding and border
        struct nk_style* s = &ctx->style;
        nk_style_push_vec2(ctx, &s->window.padding, nk_vec2(20, 20));
        nk_style_push_float(ctx, &s->window.border, 2.0f);
        nk_style_push_color(ctx, &s->window.border_color, nk_rgba(60, 60, 60, 255));

        // Set content background to be transparent
        nk_style_item transparent_background = nk_style_item_color(nk_rgba(0, 0, 0, 0));
        nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, transparent_background);

        // Center the content within the full-screen window 
        float content_height = window_height * 0.6;
        float content_width = content_height * (340.0f / 436.0f) * 2; // duck aspect ratio
        float x = (window_width - content_width) / 2;
        float y = (window_height - content_height) / 2;

        nk_layout_space_begin(ctx, NK_STATIC, window_height, 1);
        nk_layout_space_push(ctx, nk_rect(x, y, content_width, content_height));

        if (nk_group_begin(ctx, "Group", NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
            // Push transparent background for the group
            nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, transparent_background);

            nk_layout_row_dynamic(ctx, content_height * 0.4, 1);
            nk_label(ctx, label, NK_TEXT_CENTERED);

            // Set new font for the button labels
            nk_style_set_font(ctx, &fonts[2].font->handle);

            // Spacing between logo and buttons
            nk_layout_row_dynamic(ctx, 4, 1);
            
            // "Start Streaming" button
            nk_layout_row_dynamic(ctx, content_height * 0.1, 1);
            nk_label(ctx, button2Label, NK_TEXT_CENTERED);

            // "Exit" button
            nk_layout_row_dynamic(ctx, 50, 1);
            nk_label(ctx, button3Label, NK_TEXT_CENTERED);

            nk_style_pop_style_item(ctx); // pop the transparent background for the group
            nk_group_end(ctx);
            
        }

        nk_layout_space_end(ctx);

        // Pop style properties
        nk_style_pop_style_item(ctx); // pop the transparent background
        nk_style_pop_color(ctx);
        nk_style_pop_float(ctx);
        nk_style_pop_vec2(ctx);
    }
    nk_end(ctx);

    nk_style_pop_style_item(ctx); // pop the background image
}

void GUI::onDrawOverlay(ve::veEvent event) {
    if (pSubrender == nullptr) return;
 
    if (m_game->m_game_state == EGameState::RUNNING) {
        // Draw the lives bar
        drawLivesBar();
        
        // Draw inventory
        drawInventory();
    }

    if (m_game->m_game_state == EGameState::START && m_vulkan_engine->m_stream_enabled) {
        drawMainMenu("Main Menu", "Quackie in Space", 
            "Start streaming (press P)",
            "Exit (press ESC)"
        );
    }

    if (m_game->m_game_state == EGameState::LOST) {
        drawOverlayWindow("Game Lost", "GAME LOST", 
            "Restart (press R)",
            "Exit (press ESC)"
        );
    }

    if (m_game->m_game_state == EGameState::WON && m_game->CURRENT_LEVEL_NUMBER >= 3) {
        drawOverlayWindow("Game Won", "GAME WON", 
            "Restart (press R)",
            "Exit (press ESC)"
        );
    }
}
