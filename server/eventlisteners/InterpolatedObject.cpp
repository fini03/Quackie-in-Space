#include <box2d/b2_body.h>
#include <VEInclude.h>
#include "InterpolatedObject.hpp"

void InterpolatedObject::savePreviousState() {
    b2Vec2 pos = m_body->GetPosition();
    m_last_post.x = pos.x;
    m_last_post.y = pos.y;
}

void InterpolatedObject::onInterpolation(
    float frame_dt,
    float ratio 
) {
    b2Vec2 position = m_body->GetPosition();
    glm::vec3 pos = glm::vec3(position.x, position.y, 0.0f);
    m_node->setPosition(pos * ratio + m_last_post * (1.0f - ratio)); 
}

b2Body* InterpolatedObject::get_body() const {
    return m_body;
}

ve::VESceneNode* InterpolatedObject::get_node() const {
    return m_node;
}
