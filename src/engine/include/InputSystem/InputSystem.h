#ifndef input_system_h
#define input_system_h

#include "EventBus/EventBus.h"
#include "InputContext/InputContext.h"
#include "InputEvent.h"
#include "System/System.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

class InputSystem : public System
{
public:
    InputSystem(EventBus& bus, InputContext& inputContext);
    void update(UpdateContext& ctx) override;

private:
    using PlayerInputState = std::unordered_map<InputSource, bool, InputSource::Hash>;

    InputContext& context;
    std::vector<DigitalInputEvent> digitalEvents;
    std::vector<AnalogInputEvent> analogEvents;

    std::unordered_map<uint32_t, PlayerInputState> sourceStates;
    std::unordered_map<uint32_t, PlayerInputState> previousSourceStates;

    bool hasProcessedThisFrame = false;

    void processDigitalEvents(UpdateContext& ctx);
    void processAnalogEvents(UpdateContext& ctx);
};

#endif // input_system_h
