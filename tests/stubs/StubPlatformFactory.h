#ifndef stub_platform_factory_h
#define stub_platform_factory_h

#include "engine/include/IMusicFactory/IMusicFactory.h"
#include "engine/include/InputAdapter/InputAdapter.h"
#include "engine/include/IPlatformEventProvider/IPlatformEventProvider.h"
#include "engine/include/IPlatformFactory/IPlatformFactory.h"
#include "engine/include/ISoundFactory/ISoundFactory.h"

#include "StubFontFactory.h"
#include "StubRenderer.h"
#include "StubTextureFactory.h"
#include "StubWindow.h"

class StubPlatformFactory : public IPlatformFactory
{
public:
    std::unique_ptr<Window> createWindow(int w, int h, const std::string& title) override
    {
        auto win = std::make_unique<StubWindow>();
        win->create(w, h, title.c_str());
        return win;
    }

    std::unique_ptr<Renderer> createRenderer(Window& window) override
    { (void)window; return std::make_unique<StubRenderer>(); }

    InputAdapterPtr createKeyboardAdapter(EventBus& bus, const InputContext& ctx) override
    { (void)bus; (void)ctx; return nullptr; }

    InputAdapterPtr createMouseAdapter(EventBus& bus) override
    { (void)bus; return nullptr; }

    std::vector<InputAdapterPtr> createGamepadAdapters(EventBus& bus, uint32_t maxPlayers) override
    { (void)bus; (void)maxPlayers; return {}; }

    InputAdapterPtr createSystemAdapter(EventBus& bus) override
    { (void)bus; return nullptr; }

    std::unique_ptr<IPlatformEventProvider> createEventProvider() override { return nullptr; }

    std::unique_ptr<ITextureFactory> createTextureFactory(Renderer& renderer) override
    { (void)renderer; return std::make_unique<StubTextureFactory>(); }

    std::unique_ptr<ISoundFactory> createSoundFactory() override { return nullptr; }
    std::unique_ptr<IMusicFactory> createMusicFactory() override { return nullptr; }
    std::unique_ptr<IFontFactory> createFontFactory() override { return std::make_unique<StubFontFactory>(); }
};

#endif // stub_platform_factory_h
