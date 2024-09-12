#pragma once

#include "eventlisteners/PhysicsListener.hpp"
#include <VEInclude.h>
#include <glm/glm.hpp>
#include <vector>

class Tile;
class b2World;
class b2Body;

class TileManager : public PhysicsListener, public ve::VEEventListener {
private:
    std::vector<Tile*> tiles;
    b2World* m_world;
public:
    TileManager(b2World* world);
    ~TileManager();

    void create_tile(const glm::vec3& position);
    void deleteNode() override;
};
