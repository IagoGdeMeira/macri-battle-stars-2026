#ifndef movement_system_h
#define movement_system_h

#include "../System/System.h"

struct Position;
struct VelocityComponent;

class MovementSystem : public System
{
    void update(UpdateContext& context) override;
};

#endif // movement_system_h
