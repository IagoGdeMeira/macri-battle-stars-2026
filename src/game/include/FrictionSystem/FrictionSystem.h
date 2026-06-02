#ifndef friction_system_h
#define friction_system_h

#include "../../engine/include/System/System.h"

class FrictionSystem : public System
{
public:
    explicit FrictionSystem(float friction) : friction(friction) {}

    void update(UpdateContext& ctx) override;

private:
    float friction;
};

#endif // friction_system_h
