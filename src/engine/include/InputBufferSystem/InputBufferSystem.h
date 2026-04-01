#ifndef input_buffer_system_h
#define input_buffer_system_h

#include "../System/System.h"
#include "../EventBus/EventBus.h"
#include "../events/KeyEvent.h"
#include "../InputContext/InputContext.h"

#include "../../domain/components/InputBufferComponent.h"
#include "../../domain/components/PlayerComponent.h"

#include <vector>

class InputBufferSystem : public System
{
public:
    InputBufferSystem(EventBus& bus, InputContext& context);

    void update(UpdateContext& ctx) override;

private:
    InputContext& context;
    std::vector<KeyEvent> events;

    float maxBufferTime = 0.5f;
};

#endif // input_buffer_system_h
