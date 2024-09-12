#include <VEInclude.h>
#include <vector>
#include "ButtonManager.hpp"
#include "Button.hpp"
#include "game/MyVulkanEngine.hpp"
#include "traps/TrapManager.hpp"
#include "traps/Trap.hpp"

using namespace ve;

ButtonManager::ButtonManager(b2World* world, TrapManager* trap_manager) : m_world { world }, m_trap_manager{trap_manager}, m_button_count{0} {}

ButtonManager::~ButtonManager() {
    buttons.clear();
}

void ButtonManager::onPhysicsStep(float dt) {
    for(Button* button : buttons) 
        button->onPhysicsStep(dt);
}

void ButtonManager::onInterpolation(float frame_dt, float ratio) {
    for(Button* button : buttons) 
        button->onInterpolation(frame_dt, ratio);
}

void ButtonManager::reset() {
    for(Button* button : buttons) 
        button->reset();
}

void ButtonManager::deleteNode() {
    for(Button* button : buttons) {
        delete button;
    }

    buttons.clear();
}

void ButtonManager::create_button(const glm::vec3& position) {
    Button* button = new Button{m_world, position, this};
    buttons.push_back(button);
}

const std::vector<Button*>& ButtonManager::get_buttons() const {
    return buttons;
}

void ButtonManager::activate() {
    const std::vector<Trap*>& traps = m_trap_manager->get_traps();

    m_button_count++;
    
    for(Trap* trap : traps) {
        VESceneNode* node = trap->get_node();
        b2Body* body = trap->get_body();
        if(node->getName().starts_with("LaserBeam") && trap->m_enable_laser) {
            getSceneManagerPointer()->setVisibility(node, false);
            for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
                f->SetSensor(true);
            }
            trap->can_collide = false;
        }
    }
}

void ButtonManager::deactivate() {
    const std::vector<Trap*>& traps = m_trap_manager->get_traps();

    m_button_count--;
    if (m_button_count != 0)
        return;
    
    for(Trap* trap : traps) {
        VESceneNode* node = trap->get_node();
        b2Body* body = trap->get_body();
        if(node->getName().starts_with("LaserBeam") && trap->m_enable_laser) {
            getSceneManagerPointer()->setVisibility(node, true);
            for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
                f->SetSensor(false);
            }
            trap->can_collide = true;
        }
    }
}

