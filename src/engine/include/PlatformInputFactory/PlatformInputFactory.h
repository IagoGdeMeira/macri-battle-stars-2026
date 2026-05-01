#ifndef platform_input_factory_h
#define platform_input_factory_h

#include "../InputAdapter/InputAdapter.h"
#include "../InputContext/InputContext.h"

#include <memory>
#include <vector>

class EventBus;

class PlatformInputFactory
{
public:
    explicit PlatformInputFactory(EventBus& bus) : eventBus(bus) {}
    virtual ~PlatformInputFactory() = default;

    virtual std::unique_ptr<InputAdapter> createKeyboardAdapter(const InputContext& context) = 0;
    virtual std::unique_ptr<InputAdapter> createMouseAdapter() = 0;
    virtual std::vector<std::unique_ptr<InputAdapter>> createGamepadAdapters(uint32_t startingPlayerId) = 0;
    virtual std::unique_ptr<InputAdapter> createSystemAdapter() = 0;

protected:
    EventBus& eventBus;
};

#endif // platform_input_factory_h
