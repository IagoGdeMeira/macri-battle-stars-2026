#include "../../src/platform/include/SDLPlatformFactory/SDLPlatformFactory.h"

#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/platform/include/SDLRenderer/SDLRenderer.h"
#include "../../src/platform/include/SDLWindow/SDLWindow.h"

#include <catch2/catch_test_macros.hpp>
#include <SDL.h>

class SDLPlatformFactoryFixture
{
public:
    class DummyWindow : public Window
    {
    public:
        void create(int, int, const char*) override {}
        void setResolution(int, int) override {}
        void setFullscreen(bool) override {}
        void getSize(int& width, int& height) override { width = 0; height = 0; }
    };

    class DummyRenderer : public Renderer
    {
    public:
        void clear() override {}
        void present() override {}
        void drawTexture(const Texture&, const DrawTextureParams&) override {}
        void drawRectOutline(const Rectangle&, const Color&) override {}
        void drawRectFilled(const Rectangle&, const Color&) override {}
        void drawCircleOutline(const Circle&, const Color&) override {}
        void drawCircleFilled(const Circle&, const Color&) override {}
        void setViewport(const Viewport&) override {}
    };
    void configureVideoDriverForCi() const
    {
        #if defined(__linux__)
                SDL_setenv("SDL_VIDEODRIVER", "dummy", 1);
                SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
        #endif
    }

    void configureAudioDriverForCi() const
    {
        #if defined(__linux__)
                SDL_setenv("SDL_AUDIODRIVER", "dummy", 1);
        #endif
    }
};

TEST_CASE_METHOD(SDLPlatformFactoryFixture, "SDLPlatformFactory creates SDLWindow and SDLRenderer",
    "[integration][sdl_platform_factory]"
) {
    this->configureVideoDriverForCi();
    REQUIRE(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDLPlatformFactory factory;

    auto window = factory.createWindow(320, 240, "Factory Window");
    REQUIRE(window != nullptr);
    REQUIRE(dynamic_cast<SDLWindow*>(window.get()) != nullptr);

    auto renderer = factory.createRenderer(*window);
    REQUIRE(renderer != nullptr);
    REQUIRE(dynamic_cast<SDLRenderer*>(renderer.get()) != nullptr);

    SDL_Quit();
}

TEST_CASE("SDLPlatformFactory createRenderer returns nullptr for non SDLWindow",
    "[unit][sdl_platform_factory]"
) {
    SDLPlatformFactory factory;
    SDLPlatformFactoryFixture::DummyWindow window;

    auto renderer = factory.createRenderer(window);
    REQUIRE(renderer == nullptr);
}

TEST_CASE_METHOD(SDLPlatformFactoryFixture, "SDLPlatformFactory creates texture, sound and music factories",
    "[unit][sdl_platform_factory]"
) {
    this->configureVideoDriverForCi();
    this->configureAudioDriverForCi();
    REQUIRE(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0);

    SDLPlatformFactory factory;

    auto window = factory.createWindow(128, 128, "Factory Resources");
    REQUIRE(window != nullptr);

    auto renderer = factory.createRenderer(*window);
    REQUIRE(renderer != nullptr);

    auto textureFactory = factory.createTextureFactory(*renderer);
    REQUIRE(textureFactory != nullptr);

    auto soundFactory = factory.createSoundFactory();
    REQUIRE(soundFactory != nullptr);

    auto musicFactory = factory.createMusicFactory();
    REQUIRE(musicFactory != nullptr);

    SDL_Quit();
}

TEST_CASE("SDLPlatformFactory createTextureFactory returns nullptr for non SDLRenderer",
    "[unit][sdl_platform_factory]"
) {
    SDLPlatformFactory factory;
    SDLPlatformFactoryFixture::DummyRenderer renderer;

    auto textureFactory = factory.createTextureFactory(renderer);
    REQUIRE(textureFactory == nullptr);
}

TEST_CASE("SDLPlatformFactory creates keyboard, mouse, system and event provider adapters",
    "[unit][sdl_platform_factory]"
) {
    SDLPlatformFactory factory;
    EventBus bus;
    InputContext context;

    auto keyboard = factory.createKeyboardAdapter(bus, context);
    auto mouse = factory.createMouseAdapter(bus);
    auto system = factory.createSystemAdapter(bus);
    auto eventProvider = factory.createEventProvider();

    REQUIRE(keyboard != nullptr);
    REQUIRE(mouse != nullptr);
    REQUIRE(system != nullptr);
    REQUIRE(eventProvider != nullptr);
}

TEST_CASE_METHOD(SDLPlatformFactoryFixture, "SDLPlatformFactory createGamepadAdapters handles runtime environment",
    "[integration][sdl_platform_factory]"
) {
    this->configureVideoDriverForCi();
    REQUIRE(SDL_Init(SDL_INIT_JOYSTICK) == 0);

    SDLPlatformFactory factory;
    EventBus bus;

    const int joystickCount = SDL_NumJoysticks();
    auto adapters = factory.createGamepadAdapters(bus, 10);

    REQUIRE(static_cast<int>(adapters.size()) <= joystickCount);

    SDL_Quit();
}
