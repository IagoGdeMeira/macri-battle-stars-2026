#ifndef hurtbox_collision_controller_h
#define hurtbox_collision_controller_h

#include "ICollisionController/ICollisionController.h"

#include "domain/components/HurtboxControllerComponent.h"

class HurtboxCollisionController : public ICollisionController
{
public:
    bool hasMapComponent(const ControllerParams& params) const override;
    bool hasController(const ControllerParams& params) const override;
    void apply(const ControllerParams& params, StateId newState = StateId::Idle) override;
    void remove(const ControllerParams& params) override;

    void onOrientationChanged(const ControllerParams& params) override;

private:
    void activateFirstFrame(HurtboxControllerComponent& controller, World& world);
    void deactivateCurrentFrame(HurtboxControllerComponent& controller, World& world);
    void applyController(Entity entity, const HurtboxControllerComponent& newController, World& world);
};

#endif // hurtbox_collision_controller_h
