#ifndef hurtbox_controller_system_h
#define hurtbox_controller_system_h

#include "domain/components/HurtboxControllerComponent.h"

#include "engine/include/System/System.h"

class World;

class HurtboxControllerSystem : public System
{
public:
    void update(UpdateContext& ctx) override;

private:
    struct ControllerParams { HurtboxControllerComponent& controller; World& world; };

    static void updateController(const ControllerParams& params, float deltaTime);
    static void activateFrame(const ControllerParams& params, int frameIndex);
    static void deactivateFrame(const ControllerParams& params, int frameIndex);
    static void activateCurrentFrame(const ControllerParams& params);
    static void deactivateCurrentFrame(const ControllerParams& params);
    static void initialize(const ControllerParams& params);
    static bool advanceFrame(const ControllerParams& params);
};

#endif // hurtbox_controller_system_h