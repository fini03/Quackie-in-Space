#pragma once

#include <glm/glm.hpp>
#include "PhysicsListener.hpp"

class b2Body;
namespace ve {
class VESceneNode;
}

class InterpolatedObject : public PhysicsListener {
protected:
    b2Body* m_body{};
    ve::VESceneNode* m_node{};
    glm::vec3 m_last_post{};

public:
    virtual ~InterpolatedObject() {}

    virtual void savePreviousState() override;
    virtual void onInterpolation(
        float frame_dt,
        float ratio
    ) override;

    b2Body* get_body() const;
    ve::VESceneNode* get_node() const;
};
