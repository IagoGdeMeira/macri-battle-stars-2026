#ifndef pushbox_controller_system_h
#define pushbox_controller_system_h

#include "domain/components/PushboxControllerComponent.h"

#include "engine/include/System/System.h"

class World;

class PushboxControllerSystem : public System
{
public:
    void update(UpdateContext& ctx) override;

private:
    struct ControllerParams { PushboxControllerComponent& controller; World& world; };

    static void updateController(const ControllerParams& params, float deltaTime);
    static void activateFrame(const ControllerParams& params, int frameIndex);
    static void deactivateFrame(const ControllerParams& params, int frameIndex);
    static void activateCurrentFrame(const ControllerParams& params);
    static void deactivateCurrentFrame(const ControllerParams& params);
    static void initialize(const ControllerParams& params);
    static bool advanceFrame(const ControllerParams& params);
};

#endif // pushbox_controller_system_h
