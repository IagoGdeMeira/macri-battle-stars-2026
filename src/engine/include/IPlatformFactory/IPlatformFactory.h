#ifndef i_platform_factory_h
#define i_platform_factory_h

#include "../IMusicFactory/IMusicFactory.h"
#include "../InputAdapter/InputAdapter.h"
#include "../InputContext/InputContext.h"
#include "../IPlatformEventProvider/IPlatformEventProvider.h"
#include "../ISoundFactory/ISoundFactory.h"
#include "../ITextureFactory/ITextureFactory.h"
#include "../Window/Window.h"
#include "../Renderer/Renderer.h"

#include <memory>
#include <string>
#include <vector>

class EventBus;

class IPlatformFactory
{
public:
    virtual ~IPlatformFactory() = default;

    virtual std::unique_ptr<Window> createWindow(int width, int height, const std::string& title) = 0;
    virtual std::unique_ptr<Renderer> createRenderer(Window& window) = 0;

    using InputAdapterPtr = std::unique_ptr<InputAdapter>;
    virtual InputAdapterPtr createKeyboardAdapter(EventBus& bus, const InputContext& context) = 0;
    virtual InputAdapterPtr createMouseAdapter(EventBus& bus) = 0;
    virtual std::vector<InputAdapterPtr> createGamepadAdapters(EventBus& bus, uint32_t startingPlayerId) = 0;
    virtual InputAdapterPtr createSystemAdapter(EventBus& bus) = 0;
    virtual std::unique_ptr<IPlatformEventProvider> createEventProvider() = 0;

    virtual std::unique_ptr<ITextureFactory> createTextureFactory(Renderer& renderer) = 0;
    virtual std::unique_ptr<ISoundFactory> createSoundFactory() = 0;
    virtual std::unique_ptr<IMusicFactory> createMusicFactory() = 0;
};

#endif // i_platform_factory_h
