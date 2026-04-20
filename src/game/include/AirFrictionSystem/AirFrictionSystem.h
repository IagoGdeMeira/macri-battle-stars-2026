#ifndef air_friction_system_h
#define air_friction_system_h

#include "../../../engine/include/System/System.h"

class AirFrictionSystem : public System
{
public:
    explicit AirFrictionSystem(float airFriction);
    
    void update(UpdateContext& ctx) override;

private:
    float airFriction;
};

#endif // air_friction_system_h
