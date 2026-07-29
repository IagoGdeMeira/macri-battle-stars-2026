#ifndef pushbox_collision_controller_h
#define pushbox_collision_controller_h

#include "ICollisionController/ICollisionController.h"

#include "domain/components/PushboxControllerComponent.h"

class PushboxCollisionController : public ICollisionController
{
public:
    bool hasMapComponent(const ControllerParams& params) const override;
    bool hasController(const ControllerParams& params) const override;
    void apply(const ControllerParams& params, StateId newState = StateId::Idle) override;
    void remove(const ControllerParams& params) override;

    void onOrientationChanged(const ControllerParams& params) override;

private:
    void activateFirstFrame(PushboxControllerComponent& controller, World& world);
    void deactivateCurrentFrame(PushboxControllerComponent& controller, World& world);
    void applyController(Entity entity, const PushboxControllerComponent& newController, World& world);
};

#endif // pushbox_collision_controller_h
