#pragma once

#include "eventlisteners/InterpolatedObject.hpp"

class Block;
class b2World;
namespace ve {
class VESceneNode;
}

class Player : public InterpolatedObject {
private:
    ve::VESceneNode* cameraParent;

    const glm::vec3 START_POSITION{1.0f, 1.5f, 0.0f};

    void move_camera(float frame_dt);

public:
    Player(b2World* world, ve::VESceneNode* cameraParent);

    bool is_key_picked_up = false;
    Block* picked_up_block = nullptr;

    void onInterpolation(float frame_dt, float ratio) override;
    void reset() override;
};
