#pragma once

class b2Body;
class GameManager;

class Collider {
public:
    virtual ~Collider() {}

    virtual bool beginCollisionPlayer(b2Body* player) { return true; }
    virtual bool endCollisionPlayer(b2Body* player) { return true; }
    virtual void beginCollisionNPC(b2Body* npc) {}
    virtual void endCollisionNPC(b2Body* npc) {}
};
