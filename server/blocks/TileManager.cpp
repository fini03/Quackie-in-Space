#include "TileManager.hpp"
#include "Tile.hpp"
#include <vector>

TileManager::TileManager(b2World* world) : ve::VEEventListener("Tile Manager"), m_world { world } {}

TileManager::~TileManager() {
    tiles.clear();
}

void TileManager::create_tile(const glm::vec3& position) {
    Tile* tile = new Tile{m_world, position};
    tiles.push_back(tile);
}

void TileManager::deleteNode() {
    for(Tile* tile : tiles)
        delete tile;

    tiles.clear();
}
