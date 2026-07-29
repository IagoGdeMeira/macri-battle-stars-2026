#ifndef i_collision_controller_h
#define i_collision_controller_h

#include "domain/include/Entity/Entity.h"
#include "domain/value_objects/StateId/StateId.h"

class World;

class ICollisionController
{
public:
    virtual ~ICollisionController() = default;

    struct ControllerParams { Entity entity; World& world; };
    virtual bool hasMapComponent(const ControllerParams& params) const = 0;
    virtual bool hasController(const ControllerParams& params) const = 0;
    virtual void apply(const ControllerParams& params, StateId newState = StateId::Idle) = 0;
    virtual void remove(const ControllerParams& params) = 0;

    virtual void onOrientationChanged(const ControllerParams& params) = 0;
};

#endif // i_collision_controller_h
