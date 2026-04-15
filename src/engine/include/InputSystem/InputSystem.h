#ifndef input_system_h
#define input_system_h

#include "../EventBus/EventBus.h"
#include "../InputContext/InputContext.h"
#include "../System/System.h"

#include "../../events/KeyEvent.h"

#include "../../../domain/components/InputComponent.h"
#include "../../../domain/components/PlayerComponent.h"

#include <unordered_map>
#include <vector>

class InputSystem : public System
{
public:
    InputSystem(EventBus& bus, InputContext& inputContext);

    void update(UpdateContext& ctx) override;

private:
    InputContext& context;
    std::vector<KeyEvent> events;

    std::unordered_map<PlayerId, std::unordered_map<KeyCode, bool>> keyStates;
    std::unordered_map<PlayerId, std::unordered_map<KeyCode, bool>> previousKeyStates;
};

#endif // input_system_h
