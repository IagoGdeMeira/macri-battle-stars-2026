#ifndef hitbox_controller_system_h
#define hitbox_controller_system_h

#include "domain/components/HitboxControllerComponent.h"

#include "engine/include/System/System.h"

class World;

class HitboxControllerSystem : public System
{
public:
    void update(UpdateContext& ctx) override;

private:
    struct ControllerParams { HitboxControllerComponent& controller; World& world; };

    static void updateController(const ControllerParams& params, float deltaTime);
    static void activateFrame(const ControllerParams& params, int frameIndex);
    static void deactivateFrame(const ControllerParams& params, int frameIndex);
    static void activateCurrentFrame(const ControllerParams& params);
    static void deactivateCurrentFrame(const ControllerParams& params);
    static void initialize(const ControllerParams& params);
    static bool advanceFrame(const ControllerParams& params);
};

#endif // hitbox_controller_system_h
