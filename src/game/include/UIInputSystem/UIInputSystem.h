#ifndef ui_input_system_h
#define ui_input_system_h

#include "../../domain/include/Entity/Entity.h"

#include "../../engine/events/InputEvent.h"
#include "../../engine/events/MousePositionEvent.h"
#include "../../engine/include/System/System.h"
#include "../../engine/include/EventBus/EventBus.h"

#include <optional>
#include <vector>

class UIInputSystem : public System
{
public:
    UIInputSystem(EventBus& bus);

    void update(UpdateContext& ctx) override;
    void setAllowedPlayer(std::optional<uint32_t> playerId) { this->allowedPlayerId = playerId; }

private:
    EventBus& bus;
    std::optional<uint32_t> allowedPlayerId;
    std::optional<Entity> focusedEntity;
    Position mousePosition {0.f, 0.f};

    std::vector<DigitalInputEvent> digitalEvents;

    struct Direction { int dx = 0, dy = 0; };

    void navigate(UpdateContext& ctx, Direction dir);

    void navigateKeyboard(UpdateContext& ctx, KeyCode key);
    void navigateGamepad(UpdateContext& ctx, GamepadButton button);
    void navigateMouse(UpdateContext& ctx, MouseButton button);

    void activate(UpdateContext& ctx);
    std::optional<Entity> findClosest(UpdateContext& ctx, Entity current, Direction dir) const;
};

#endif // ui_input_system_h
