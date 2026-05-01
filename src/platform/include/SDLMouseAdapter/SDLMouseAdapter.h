#ifndef sdl_mouse_adapter_h
#define sdl_mouse_adapter_h

#include "../../engine/include/InputAdapter/InputAdapter.h"

#include <memory>

class EventBus;

class SDLMouseAdapter : public InputAdapter
{
public:
    SDLMouseAdapter(EventBus& eventBus, uint32_t assignedPlayer);
    
    void processEvents(const std::vector<std::unique_ptr<PlatformEvent>>& events) override;

private:
    EventBus& eventBus;
    uint32_t playerId;
};

#endif // sdl_mouse_adapter_h
