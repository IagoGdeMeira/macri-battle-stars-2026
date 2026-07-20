#ifndef gravity_system_h
#define gravity_system_h

#include "engine/include/System/System.h"

class GravitySystem : public System
{
public:
    explicit GravitySystem(float baseGravity) : baseGravity(baseGravity) {}
    void update(UpdateContext& ctx) override;

private:
    float baseGravity;
};

#endif // gravity_system_h
