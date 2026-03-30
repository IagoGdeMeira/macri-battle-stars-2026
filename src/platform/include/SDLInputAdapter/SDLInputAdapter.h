#ifndef sdl_input_adapter_h
#define sdl_input_adapter_h

#include "../../engine/include/InputAdapter/InputAdapter.h"

class EventBus;

class SDLInputAdapter : public InputAdapter
{
public:
    SDLInputAdapter(EventBus& eventBus) : eventBus(eventBus) {}

    void poll() override;

private:
    EventBus& eventBus;
};

#endif // sdl_input_adapter_h
