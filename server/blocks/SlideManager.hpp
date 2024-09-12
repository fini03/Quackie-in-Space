#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "eventlisteners/PhysicsListener.hpp"

class Slide;
class b2World;
class b2Body;
class GameManager;

class SlideManager : public PhysicsListener {
private:
    std::vector<Slide*> slides;
    b2World* m_world;

public:
    SlideManager(b2World* world);
    ~SlideManager();

    void create_slide(const glm::vec3& position);
    const std::vector<Slide*>& get_slides() const;

    void savePreviousState() override;
    void onPhysicsStep(float dt) override;
    void onInterpolation(float frame_dt, float ratio) override;
    void reset() override;
    void deleteNode() override;
};
