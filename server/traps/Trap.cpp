#include "Trap.hpp"

Trap::Trap(b2World* world, const glm::vec3& position, GameManager* game_manager) : m_game{game_manager} {}

Trap::~Trap() {}
