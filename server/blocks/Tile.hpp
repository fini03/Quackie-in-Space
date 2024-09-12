#pragma once

#include <glm/glm.hpp>

class b2World;
class b2Body;
class GameManager;

namespace ve {
    class VESceneNode;
}

class Tile {
private:
    b2Body* m_body;
    ve::VESceneNode* m_node;
public:
    Tile(b2World* world, const glm::vec3& position);
    ~Tile();
};
