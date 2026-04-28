#ifndef sdl_mouse_adapter_h
#define sdl_mouse_adapter_h

#include "../../engine/include/InputAdapter/InputAdapter.h"
#include <unordered_map>

class EventBus;

class SDLMouseAdapter : public InputAdapter
{
public:
    SDLMouseAdapter(EventBus& eventBus, uint32_t assignedPlayer);
    void poll() override;

private:
    EventBus& eventBus;
    uint32_t playerId;
    std::unordered_map<MouseButton, bool> previousButtonState;
};

#endif // sdl_mouse_adapter_h
