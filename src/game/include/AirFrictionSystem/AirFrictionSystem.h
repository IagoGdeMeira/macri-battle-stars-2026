#ifndef air_friction_system_h
#define air_friction_system_h

#include "domain/value_objects/Geometry/Geometry.h"

#include "engine/include/System/System.h"

class AirFrictionSystem : public System
{
public:
    explicit AirFrictionSystem(float airFriction) : airFriction(airFriction) {}
    
    void update(UpdateContext& ctx) override;
    void setNegativePositionToZero(Position& pos);

private:
    float airFriction;
};

#endif // air_friction_system_h
