#include "SDLPlatformFactory/SDLPlatformFactory.h"

#include "SDLFontFactory/SDLFontFactory.h"
#include "SDLGamepadAdapter/SDLGamepadAdapter.h"
#include "SDLKeyboardAdapter/SDLKeyboardAdapter.h"
#include "SDLMouseAdapter/SDLMouseAdapter.h"
#include "SDLMusicFactory/SDLMusicFactory.h"
#include "SDLPlatformEventProvider/SDLPlatformEventProvider.h"
#include "SDLRenderer/SDLRenderer.h"
#include "SDLSoundFactory/SDLSoundFactory.h"
#include "SDLSystemAdapter/SDLSystemAdapter.h"
#include "SDLTextureFactory/SDLTextureFactory.h"
#include "SDLWindow/SDLWindow.h"

#include <SDL.h>

std::unique_ptr<Window> SDLPlatformFactory::createWindow(int width, int height, const std::string& title)
{
    auto window = std::make_unique<SDLWindow>();
    window->create(width, height, title.c_str());
    return window;
}

std::unique_ptr<Renderer> SDLPlatformFactory::createRenderer(Window& window)
{
    auto* sdlWindow = dynamic_cast<SDLWindow*>(&window);
    if (!sdlWindow) return nullptr;
    return std::make_unique<SDLRenderer>(sdlWindow->get());
}

std::unique_ptr<InputAdapter> SDLPlatformFactory::createKeyboardAdapter(EventBus& bus, const InputContext& context)
{ return std::make_unique<SDLKeyboardAdapter>(bus, context); }

std::unique_ptr<InputAdapter> SDLPlatformFactory::createMouseAdapter(EventBus& bus)
{ return std::make_unique<SDLMouseAdapter>(bus, 0); }

std::vector<std::unique_ptr<InputAdapter>> SDLPlatformFactory::createGamepadAdapters(
    EventBus& bus, uint32_t startingPlayerId
) {
    std::vector<std::unique_ptr<InputAdapter>> adapters;
    int numJoysticks = SDL_NumJoysticks();
    for (int i = 0; i < numJoysticks; ++i)
    {
        try { adapters.push_back(std::make_unique<SDLGamepadAdapter>(bus, startingPlayerId + i, i)); }
        catch (const std::exception&) {}
    }
    return adapters;
}

std::unique_ptr<InputAdapter> SDLPlatformFactory::createSystemAdapter(EventBus& bus)
{ return std::make_unique<SDLSystemAdapter>(bus); }

std::unique_ptr<IPlatformEventProvider> SDLPlatformFactory::createEventProvider()
{ return std::make_unique<SDLPlatformEventProvider>(); }

std::unique_ptr<ITextureFactory> SDLPlatformFactory::createTextureFactory(Renderer& renderer)
{
    auto* sdlRenderer = dynamic_cast<SDLRenderer*>(&renderer);
    if (!sdlRenderer) return nullptr;
    return std::make_unique<SDLTextureFactory>(sdlRenderer->get());
}

std::unique_ptr<ISoundFactory> SDLPlatformFactory::createSoundFactory() { return std::make_unique<SDLSoundFactory>(); }

std::unique_ptr<IMusicFactory> SDLPlatformFactory::createMusicFactory() { return std::make_unique<SDLMusicFactory>(); }

std::unique_ptr<IFontFactory> SDLPlatformFactory::createFontFactory() { return std::make_unique<SDLFontFactory>(); }
