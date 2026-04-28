#ifndef sdl_input_adapter_h
#define sdl_input_adapter_h

#include "../../engine/include/InputAdapter/InputAdapter.h"
#include "../../engine/include/InputSource/InputSource.h"

#include <cstdint>
#include <unordered_map>

class EventBus;
class InputContext;

class SDLKeyboardAdapter : public InputAdapter
{
public:
    SDLKeyboardAdapter(EventBus& eventBus, const InputContext& inputContext);
    void poll() override;

private:
    EventBus& eventBus;
    std::unordered_map<KeyCode, uint32_t> keyToPlayer;
    void buildKeyPlayerMap(const InputContext& context);
};

#endif // sdl_input_adapter_h
