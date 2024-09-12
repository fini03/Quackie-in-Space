#include <box2d/b2_math.h>
#include <box2d/box2d.h>
#include <VEInclude.h>
#include "Player.hpp"

using namespace ve;

Player::Player(b2World* world, ve::VESceneNode* cameraParent) : cameraParent{cameraParent} {
    VECHECKPOINTER(m_node = ve::getSceneManagerPointer()->loadModel("Duck", "media/models/Duck", "duck_smol.obj", 0, ve::getRoot()));

    m_node->setPosition(START_POSITION);
    m_node->setTransform(glm::rotate(m_node->getTransform(), glm::radians(180.0f), glm::vec3(0, 1, 0)));

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(START_POSITION.x, START_POSITION.y);
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(m_node);
    
    m_body = world->CreateBody(&bodyDef);
    m_body->SetFixedRotation(true);

    // Create capsule collider
    float radius = 0.15f;
    b2FixtureDef fixtureDef;
    
    // Top circle
    b2CircleShape circleTop;
    circleTop.m_radius = radius;
    circleTop.m_p.Set(0.25f, 0.25);

    // Bottom left circle
    b2CircleShape circleBottomLeft;
    circleBottomLeft.m_radius = radius;
    circleBottomLeft.m_p.Set(-0.25f, -0.25f);

    // Bottom circle
    b2CircleShape circleBottomRight;
    circleBottomRight.m_radius = radius;
    circleBottomRight.m_p.Set(0.25f, -0.25f);

    // Middle box
    b2PolygonShape dynamicBox{};
    dynamicBox.SetAsBox(0.3, 0.35, { 0.0f, 0.05f }, 0.0f);

    // Make all fixtures have the same density, total mass
    // should sum up to around 100 
    fixtureDef.density = 180.0f;
    fixtureDef.friction = 0.4f;

    // Attach fixtures to body
    //fixtureDef.shape = &circleTop;
    //duck->CreateFixture(&fixtureDef);
    fixtureDef.shape = &dynamicBox;
    m_body->CreateFixture(&fixtureDef);
    fixtureDef.shape = &circleBottomLeft;
    m_body->CreateFixture(&fixtureDef);
    fixtureDef.shape = &circleBottomRight;
    m_body->CreateFixture(&fixtureDef);

    b2PolygonShape polygonShape;
    polygonShape.SetAsBox(0.25, 0.4);
    fixtureDef.shape = &polygonShape;
    fixtureDef.isSensor = true;
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>((void*)3);
    m_body->CreateFixture(&fixtureDef);
}

void Player::move_camera(float frame_dt) {
    const float FRAME_WIDTH = 10.0f;
    const float FRAME_HEIGHT = 10.0f;
    const float CAMERA_SPEED = 5.0f;
    const float CAMERA_MARGIN_X = 5.0f;
    const float CAMERA_MARGIN_Y = 4.0f;

    glm::vec3 camera_position = cameraParent->getPosition();

    const float FRAME_LEFT = camera_position.x - FRAME_WIDTH / 2;
    const float FRAME_RIGHT = camera_position.x + FRAME_WIDTH / 2;
    const float FRAME_TOP = camera_position.y + FRAME_HEIGHT / 2;
    const float FRAME_BOTTOM = camera_position.y - FRAME_HEIGHT / 2;

    const b2Vec2& duck_position = m_body->GetPosition();

    const float DISTANCE_TO_LEFT = duck_position.x - FRAME_LEFT;
    const float DISTANCE_TO_RIGHT = FRAME_RIGHT - duck_position.x;
    const float DISTANCE_TO_TOP = FRAME_TOP - duck_position.y;
    const float DISTANCE_TO_BOTTOM = duck_position.y - FRAME_BOTTOM;

    float camera_movement_x = 0.0f;
    float camera_movement_y = 0.0f;

    if (DISTANCE_TO_LEFT < CAMERA_MARGIN_X) {
        camera_movement_x = -1.0f * (CAMERA_MARGIN_X - DISTANCE_TO_LEFT) * CAMERA_SPEED;
    } else if (DISTANCE_TO_RIGHT < CAMERA_MARGIN_X) {
        camera_movement_x = (CAMERA_MARGIN_X - DISTANCE_TO_RIGHT) * CAMERA_SPEED;
    }

    if (DISTANCE_TO_BOTTOM < CAMERA_MARGIN_Y) {
        camera_movement_y = -1.0f * (CAMERA_MARGIN_Y - DISTANCE_TO_BOTTOM) * CAMERA_SPEED;
    } else if (DISTANCE_TO_TOP < CAMERA_MARGIN_Y) {
        camera_movement_y = (CAMERA_MARGIN_Y - DISTANCE_TO_TOP) * CAMERA_SPEED;
    }

    camera_position.x += camera_movement_x * frame_dt;
    camera_position.y += camera_movement_y * frame_dt;

    cameraParent->setPosition(camera_position);
}

void Player::onInterpolation(float frame_dt, float ratio) {
    InterpolatedObject::onInterpolation(frame_dt, ratio);
    move_camera(frame_dt);
}

void Player::reset() {
    glm::vec3 camera_position = cameraParent->getPosition();
    camera_position.x = 1.0f;
    camera_position.y = 2.4f;
    cameraParent->setPosition(camera_position);

    m_body->SetLinearVelocity(b2Vec2{0.0f, 0.0f});
    m_body->SetTransform(b2Vec2{START_POSITION.x, START_POSITION.y}, m_body->GetAngle());
    m_last_post = glm::vec3(START_POSITION.x, START_POSITION.y, 0.0f);
    is_key_picked_up = false;
    picked_up_block = nullptr;
    m_body->SetAwake(true);
}
