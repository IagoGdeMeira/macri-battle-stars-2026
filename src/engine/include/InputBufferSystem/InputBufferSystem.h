#ifndef input_buffer_system_h
#define input_buffer_system_h

#include "EventBus/EventBus.h"
#include "InputContext/InputContext.h"
#include "InputEvent.h"
#include "System/System.h"

#include <vector>

class InputBufferSystem : public System
{
public:
    InputBufferSystem(EventBus& bus, InputContext& context);
    void update(UpdateContext& ctx) override;

private:
    InputContext& context;
    std::vector<DigitalInputEvent> events;
    float maxBufferTime = 0.5f;
};

#endif // input_buffer_system_h
