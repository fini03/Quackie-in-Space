#include <box2d/box2d.h>
#include <VEInclude.h>
#include "ButtonManager.hpp"
#include "Button.hpp"

using namespace ve;

Button::Button(b2World* world, const glm::vec3& position, ButtonManager* manager) : m_manager{manager}, pressed_count{0} {
    STARTING_POSITION = b2Vec2{position.x, position.y};
    b2BodyDef bodyDef{};

    // Create scene node and attach to body
    VECHECKPOINTER(m_node = getSceneManagerPointer()->loadModel("Button" + std::to_string(position.x) + "-" + std::to_string(position.y), "media/models/Button", "cube.obj", 0, getRoot()));

    // Create a scaling matrix
    glm::mat4 scaleMatrix = glm::scale(glm::vec3(0.50f, 0.50f, 0.50f));

    // Apply the scaling matrix to the object's transform
    m_node->setTransform(scaleMatrix);
    m_node->setPosition(glm::vec3(position.x, position.y - 0.5f, position.z));

    bodyDef.position.Set(position.x, position.y - 0.5f);
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);

    // Create body and fixture
    m_body = world->CreateBody(&bodyDef);

    b2PolygonShape polygonShape;
    polygonShape.SetAsBox(0.2, 0.2);
    b2FixtureDef fixtureDef{};
    fixtureDef.shape = &polygonShape;
    fixtureDef.isSensor = true;
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
    m_body->CreateFixture(&fixtureDef);
}

Button::~Button() {
    getSceneManagerPointer()->deleteSceneNodeAndChildren(m_node->getName());
    b2World* world = m_body->GetWorld();
    world->DestroyBody( m_body );
}

void Button::savePreviousState() {}

void Button::onPhysicsStep(float /* dt */) {}

void Button::onInterpolation(float frame_dt, float ratio) {
    getSceneManagerPointer()->setVisibility(m_node, pressed_count == 0);
}

void Button::reset() {
    m_body->SetTransform(STARTING_POSITION, m_body->GetAngle());
    getSceneManagerPointer()->setVisibility(m_node, false);
    pressed_count = 0;
    m_manager->m_button_count = 0;
    m_manager->deactivate();
}

bool Button::beginCollisionPlayer(b2Body* /* player */) {
    pressed_count++;
    m_manager->activate();
    return false;
}

bool Button::endCollisionPlayer(b2Body* /* player */) {
    pressed_count--;
    m_manager->deactivate();
    return false;
}

void Button::beginCollisionNPC(b2Body* npc) {
    beginCollisionPlayer(npc);
}

void Button::endCollisionNPC(b2Body* npc) {
    endCollisionPlayer(npc);
}
