#ifndef sdl_input_adapter_h
#define sdl_input_adapter_h

#include "../../engine/include/InputAdapter/InputAdapter.h"
#include "../../engine/include/InputSource/InputSource.h"

#include <cstdint>
#include <memory>
#include <unordered_map>

class EventBus;
struct InputContext;

class SDLKeyboardAdapter : public InputAdapter
{
public:
    SDLKeyboardAdapter(EventBus& eventBus, const InputContext& inputContext);

    void processEvents(const std::vector<std::unique_ptr<PlatformEvent>>& events) override;

private:
    EventBus& eventBus;
    std::unordered_map<KeyCode, uint32_t> keyToPlayer;
    
    void buildKeyPlayerMap(const InputContext& context);
};

#endif // sdl_input_adapter_h
