#include "Tile.hpp"
#include <VEInclude.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <glm/glm.hpp>

using namespace ve;

Tile::Tile(b2World* world, const glm::vec3& position) {
    b2BodyDef bodyDef{};

    b2PolygonShape dynamicBox{};
    dynamicBox.SetAsBox(0.5f, 0.5f);

    b2FixtureDef fixtureDef{};
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 0.0f;
    fixtureDef.friction = 0.5f;
    
    VECHECKPOINTER(m_node = getSceneManagerPointer()->loadModel("Tiles" + std::to_string(position.x) + "-" + std::to_string(position.y), "media/models/Tiles", "cube.obj", 0, getRoot()));
            
    bodyDef.position.Set(position.x, position.y);
    m_node->setPosition(position);
    
    m_body = world->CreateBody(&bodyDef);
    m_body->CreateFixture(&fixtureDef);
}

Tile::~Tile() {
    getSceneManagerPointer()->deleteSceneNodeAndChildren(m_node->getName());
    b2World* world = m_body->GetWorld();
    world->DestroyBody( m_body );
}
