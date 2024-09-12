#include <box2d/box2d.h>
#include <cstdint>
#include "CollisionListener.hpp"
#include "Collider.hpp"
#include "game/Player.hpp"
#include "Keys.hpp"

CollisionListener::CollisionListener(b2World* world, Player* player, Keys* physics_keys_listener) :
    m_physics_keys_listener{ physics_keys_listener }, m_player{ player }, m_world { world } {}

CollisionListener::~CollisionListener() {}

void CollisionListener::BeginContact(b2Contact* contact) {
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    // Get the bodies involved in the contact
    b2Body* bodyA = fixtureA->GetBody();
    b2Body* bodyB = fixtureB->GetBody();

    // Check if fixture A or B was the foot sensor
    uintptr_t ptr_fixture_user_data_A = fixtureA->GetUserData().pointer;
    uintptr_t ptr_fixture_user_data_B = fixtureB->GetUserData().pointer;

    bool is_player_A = bodyA == m_player->get_body();
    bool is_player_B = bodyB == m_player->get_body();
    bool touched_ground = true;

    if ((ptr_fixture_user_data_A > 0x100 || is_player_A) && (ptr_fixture_user_data_B > 0x100 || is_player_B)) {
        if (is_player_A) {
            auto collider = reinterpret_cast<Collider*>(ptr_fixture_user_data_B);
            touched_ground = collider->beginCollisionPlayer(bodyA);
        } else if (is_player_B) {
            auto collider = reinterpret_cast<Collider*>(ptr_fixture_user_data_A);
            touched_ground = collider->beginCollisionPlayer(bodyB);
        } else if (bodyA->GetType() == b2_dynamicBody) {
            auto collider = reinterpret_cast<Collider*>(ptr_fixture_user_data_B);
            collider->beginCollisionNPC(bodyA);
        } else {
            // Collision is only happening between dynamic bodies and
            // everything else, so this means bodyB is a dynamic body
            auto collider = reinterpret_cast<Collider*>(ptr_fixture_user_data_A);
            collider->beginCollisionNPC(bodyB);
        }
    }

    if(!is_player_A && !is_player_B) return;

    int fixture_user_data_A = static_cast<int>(ptr_fixture_user_data_A);
    int fixture_user_data_B = static_cast<int>(ptr_fixture_user_data_B);
    
    // Can i jump
    if ((fixture_user_data_A == 3 || fixture_user_data_B == 3) && touched_ground) {
        m_physics_keys_listener->move_transition(ETransition::HIT_GROUND);
        return;
    }
}

void CollisionListener::EndContact(b2Contact* contact) {
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    // Get the bodies involved in the contact
    b2Body* bodyA = fixtureA->GetBody();
    b2Body* bodyB = fixtureB->GetBody();

    // Check if fixture A or B was the foot sensor
    uintptr_t ptr_fixture_user_data_A = fixtureA->GetUserData().pointer;
    uintptr_t ptr_fixture_user_data_B = fixtureB->GetUserData().pointer;

    bool is_player_A = bodyA == m_player->get_body();
    bool is_player_B = bodyB == m_player->get_body();

    if ((ptr_fixture_user_data_A > 0x100 || is_player_A) && (ptr_fixture_user_data_B > 0x100 || is_player_B)) {
        if (is_player_A) {
            auto collider = reinterpret_cast<Collider*>(ptr_fixture_user_data_B);
            collider->endCollisionPlayer(bodyA);
        } else if (is_player_B) {
            auto collider = reinterpret_cast<Collider*>(ptr_fixture_user_data_A);
            collider->endCollisionPlayer(bodyB);
        } else if (bodyA->GetType() == b2_dynamicBody) {
            auto collider = reinterpret_cast<Collider*>(ptr_fixture_user_data_B);
            collider->endCollisionNPC(bodyA);
        } else {
            // Collision is only happening between dynamic bodies and
            // everything else, so this means bodyB is a dynamic body
            auto collider = reinterpret_cast<Collider*>(ptr_fixture_user_data_A);
            collider->endCollisionNPC(bodyB);
        }
        return;
    }
}
