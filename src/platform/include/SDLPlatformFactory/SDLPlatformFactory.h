#ifndef sdl_platform_factory_h
#define sdl_platform_factory_h

#include "../../engine/include/IPlatformFactory/IPlatformFactory.h"

class SDLPlatformFactory : public IPlatformFactory
{
public:
    std::unique_ptr<Window> createWindow(int width, int height, const std::string& title) override;
    std::unique_ptr<Renderer> createRenderer(Window& window) override;

    using InputAdapterPtr = std::unique_ptr<InputAdapter>;
    InputAdapterPtr createKeyboardAdapter(EventBus& bus, const InputContext& context) override;
    InputAdapterPtr createMouseAdapter(EventBus& bus) override;
    std::vector<InputAdapterPtr> createGamepadAdapters(EventBus& bus, uint32_t startingPlayerId) override;
    InputAdapterPtr createSystemAdapter(EventBus& bus) override;
    std::unique_ptr<IPlatformEventProvider> createEventProvider() override;

    std::unique_ptr<ITextureFactory> createTextureFactory(Renderer& renderer) override;
    std::unique_ptr<ISoundFactory> createSoundFactory() override;
    std::unique_ptr<IMusicFactory> createMusicFactory() override;
    std::unique_ptr<IFontFactory> createFontFactory() override;
};

#endif // sdl_platform_factory_h
