#ifndef sdl_gamepad_adapter_h
#define sdl_gamepad_adapter_h

#include "../../engine/include/InputAdapter/InputAdapter.h"

#include <cstdint>
#include <memory>
#include <SDL.h>

class EventBus;

class SDLGamepadAdapter : public InputAdapter
{
public:
    SDLGamepadAdapter(EventBus& eventBus, uint32_t assignedPlayer, int joystickIndex);
    ~SDLGamepadAdapter() override;

    void processEvents(const std::vector<std::unique_ptr<PlatformEvent>>& events) override;

private:
    EventBus& eventBus;
    uint32_t playerId;
    SDL_Joystick* joystick;
};

#endif // sdl_gamepad_adapter_h
