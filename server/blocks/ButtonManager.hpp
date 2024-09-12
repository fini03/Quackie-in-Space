#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "eventlisteners/PhysicsListener.hpp"

class Button;
class b2World;
class TrapManager;

class ButtonManager : public PhysicsListener {
private:
    std::vector<Button*> buttons;
    b2World* m_world;
    TrapManager* m_trap_manager;

public:
    ButtonManager(b2World* world, TrapManager* trap_manager);
    ~ButtonManager();

    int m_button_count;

    void create_button(const glm::vec3& position);
    void activate();
    void deactivate();

    void onPhysicsStep(float df) override;
    void onInterpolation(float frame_dt, float ratio) override;
    void reset() override;
    void deleteNode() override;

    const std::vector<Button*>& get_buttons() const;
};
