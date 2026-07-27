#ifndef collision_controller_system_h
#define collision_controller_system_h

#include "StateChangedEvent.h"

#include "engine/include/System/System.h"

#include <vector>

class EventBus;
class World;

struct HitboxControllerComponent;
struct HurtboxControllerComponent;
struct PushboxControllerComponent;
struct ActiveComponent;

class CollisionControllerSystem : public System
{
public:
    explicit CollisionControllerSystem(EventBus& bus);
    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;
    std::vector<StateChangedEvent> pendingEvents;

    struct HitboxParams { World& world; HitboxControllerComponent& controller; };
    void activateFirstFrame(const HitboxParams& params);
    void deactivateCurrentFrame(const HitboxParams& params);
    void applyHitboxController(const HitboxParams& params, Entity entity);

    struct HurtboxParams { World& world; HurtboxControllerComponent& controller; };
    void activateFirstFrame(const HurtboxParams& params);
    void deactivateCurrentFrame(const HurtboxParams& params);
    void applyHurtboxController(const HurtboxParams& params, Entity entity);

    struct PushboxParams { World& world; PushboxControllerComponent& controller; };
    void activateFirstFrame(const PushboxParams& params);
    void deactivateCurrentFrame(const PushboxParams& params);
    void applyPushboxController(const PushboxParams& params, Entity entity);
};

#endif // collision_controller_system_h
