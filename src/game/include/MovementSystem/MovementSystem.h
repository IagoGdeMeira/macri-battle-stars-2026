#ifndef movement_system_h
#define movement_system_h

#include "engine/include/System/System.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

class MovementSystem : public System
{
public:
    void update(UpdateContext& ctx) override;
};

#endif // movement_system_h
