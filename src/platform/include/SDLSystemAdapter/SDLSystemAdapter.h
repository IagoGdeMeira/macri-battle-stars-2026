#ifndef sdl_system_adapter_h
#define sdl_system_adapter_h

#include "engine/include/InputAdapter/InputAdapter.h"

class EventBus;

class SDLSystemAdapter : public InputAdapter
{
public:
    explicit SDLSystemAdapter(EventBus& bus) : eventBus(bus) {}
    void processEvents(const std::vector<InputAdapter::Event>& events) override;

private:
    EventBus& eventBus;
};

#endif // sdl_system_adapter_h
