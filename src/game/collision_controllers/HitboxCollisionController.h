#ifndef hitbox_collision_controller_h
#define hitbox_collision_controller_h

#include "ICollisionController/ICollisionController.h"

#include "domain/components/HitboxControllerComponent.h"

class HitboxCollisionController : public ICollisionController
{
public:
    bool hasMapComponent(const ControllerParams& params) const override;
    bool hasController(const ControllerParams& params) const override;
    void apply(const ControllerParams& params, StateId newState = StateId::Idle) override;
    void remove(const ControllerParams& params) override;

private:
    void activateFirstFrame(HitboxControllerComponent& controller, World& world);
    void deactivateCurrentFrame(HitboxControllerComponent& controller, World& world);
    void applyController(Entity entity, const HitboxControllerComponent& newController, World& world);
};

#endif // hitbox_collision_controller_h
