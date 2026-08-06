#ifndef gravity_system_h
#define gravity_system_h

#include "domain/components/GravityComponent.h"
#include "domain/components/JumpComponent.h"
#include "domain/components/VelocityComponent.h"

#include "engine/include/System/System.h"

class GravitySystem : public System
{
public:
    explicit GravitySystem(float baseGravity) : baseGravity(baseGravity) {}
    void update(UpdateContext& ctx) override;

private:
    float baseGravity;

    float computeGravityScale(const GravityComponent& gravity, const JumpComponent* jump, float velocityY) const;
    void applyGravity(VelocityComponent& velocity, float deltaTime, float scale) const;
};

#endif // gravity_system_h
