#pragma once

class PhysicsListener {
public:
    virtual ~PhysicsListener() {}
    virtual void savePreviousState() {}
    virtual void onPhysicsStep(float dt) {}
    virtual void onInterpolation(float frame_dt, float ratio) {}
    virtual void reset() {}
    virtual void deleteNode() {}
};
