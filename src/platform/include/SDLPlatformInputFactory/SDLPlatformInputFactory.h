#ifndef sdl_platform_input_factory_h
#define sdl_platform_input_factory_h

#include "../../engine/include/PlatformInputFactory/PlatformInputFactory.h"

class SDLPlatformInputFactory : public PlatformInputFactory
{
public:
    explicit SDLPlatformInputFactory(EventBus& bus) : PlatformInputFactory(bus) {}

    std::unique_ptr<InputAdapter> createKeyboardAdapter(const InputContext& context) override;
    std::unique_ptr<InputAdapter> createMouseAdapter() override;
    std::vector<std::unique_ptr<InputAdapter>> createGamepadAdapters(uint32_t startingPlayerId) override;
    std::unique_ptr<InputAdapter> createSystemAdapter() override;
};

#endif // sdl_platform_input_factory_h
