#include "platform/include/SDLRenderer/SDLRenderer.h"

#include "domain/value_objects/Color/Color.h"

#include "engine/value_objects/Viewport/Viewport.h"

#include "platform/include/SDLTexture/SDLTexture.h"
#include "platform/include/SDLWindow/SDLWindow.h"
#include "platform/include/SDLFont/SDLFont.h"

#include <catch2/catch_test_macros.hpp>
#include <SDL.h>
#include <SDL_ttf.h>
#include <fstream>
#include <memory>

class SDLRendererFixture
{
public:
    void configureVideoDriverForCi() const
    {
        #if defined(__linux__)
            SDL_setenv("SDL_VIDEODRIVER", "dummy", 1);
            SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
        #endif
    }
};

TEST_CASE_METHOD(SDLRendererFixture, "SDLRenderer can be created from an SDL window",
    "[integration][sdl_renderer]"
) {
    this->configureVideoDriverForCi();
    REQUIRE(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDLWindow window;
    window.create(800, 600, "Renderer Test");

    SDLRenderer renderer(window.get());

    REQUIRE_NOTHROW(renderer.clear());
    REQUIRE_NOTHROW(renderer.present());
    REQUIRE_NOTHROW(renderer.setViewport(Viewport{0, 0, 800, 600}));

    SDL_Quit();
}

TEST_CASE_METHOD(SDLRendererFixture, "SDLRenderer draws textures", "[unit][sdl_renderer]")
{
    this->configureVideoDriverForCi();
    REQUIRE(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDLWindow window;
    window.create(800, 600, "Texture Rendering Test");

    SDLRenderer renderer(window.get());
    SDLTexture dummyTexture(nullptr);
    auto texturePtr = std::make_shared<SDLTexture>(std::move(dummyTexture));

    DrawTextureCommand cmd;
    cmd.texture = texturePtr;
    cmd.dest.position.x = 10.f;
    cmd.dest.position.y = 20.f;
    cmd.dest.size.width = 32.f;
    cmd.dest.size.height = 48.f;
    cmd.rotation = 30.f;
    cmd.flipX = true;
    cmd.flipY = false;

    REQUIRE_NOTHROW(renderer.clear());
    REQUIRE_NOTHROW(renderer.draw(cmd));
    REQUIRE_NOTHROW(renderer.present());

    SDL_Quit();
}

TEST_CASE_METHOD(SDLRendererFixture, "SDLRenderer draws rectangles", "[unit][sdl_renderer]")
{
    this->configureVideoDriverForCi();
    REQUIRE(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDLWindow window;
    window.create(800, 600, "Rectangle Rendering Test");

    SDLRenderer renderer(window.get());

    DrawRectangleCommand cmd1;
    cmd1.rect = Rectangle{{0.f, 0.f}, 10.f, 10.f};
    cmd1.color = Color{};
    cmd1.filled = false;
    cmd1.layer = 0;
    cmd1.zIndex = 0;
    cmd1.order = 0;

    DrawRectangleCommand cmd2;
    cmd2.rect = Rectangle{{1.f, 1.f}, 12.f, 14.f};
    cmd2.color = Color{10, 20, 30, 40};
    cmd2.filled = true;
    cmd2.layer = 0;
    cmd2.zIndex = 0;
    cmd2.order = 0;

    REQUIRE_NOTHROW(renderer.clear());
    REQUIRE_NOTHROW(renderer.draw(cmd1));
    REQUIRE_NOTHROW(renderer.draw(cmd2));
    REQUIRE_NOTHROW(renderer.present());

    SDL_Quit();
}

TEST_CASE_METHOD(SDLRendererFixture, "SDLRenderer draws circles", "[unit][sdl_renderer]")
{
    this->configureVideoDriverForCi();
    REQUIRE(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDLWindow window;
    window.create(800, 600, "Circle Rendering Test");

    SDLRenderer renderer(window.get());

    DrawCircleCommand cmd1;
    cmd1.circle = Circle{{4.f, 5.f}, 6.f};
    cmd1.color = Color{};
    cmd1.filled = false;
    cmd1.layer = 0;
    cmd1.zIndex = 0;
    cmd1.order = 0;

    DrawCircleCommand cmd2;
    cmd2.circle = Circle{{7.f, 8.f}, 9.f};
    cmd2.color = Color{10, 20, 30, 40};
    cmd2.filled = true;
    cmd2.layer = 0;
    cmd2.zIndex = 0;
    cmd2.order = 0;

    REQUIRE_NOTHROW(renderer.clear());
    REQUIRE_NOTHROW(renderer.draw(cmd1));
    REQUIRE_NOTHROW(renderer.draw(cmd2));
    REQUIRE_NOTHROW(renderer.present());

    SDL_Quit();
}

TEST_CASE_METHOD(SDLRendererFixture, "SDLRenderer has drawFont method defined", "[integration][sdl_renderer]")
{
    this->configureVideoDriverForCi();
    REQUIRE(SDL_Init(SDL_INIT_VIDEO) == 0);
    REQUIRE(TTF_Init() == 0);

    #if defined(_WIN32)
        const std::string fontPath = "C:\\Windows\\Fonts\\arial.ttf";
    #else
        const std::string fontPath = "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf";
    #endif

    std::ifstream fontFile(fontPath);
    if (!fontFile.good())
    {
        TTF_Quit();
        SDL_Quit();
        SKIP("Font file not available");
    }

    std::string textStr = "Hello";
    Rectangle textRect{{50.f, 50.f}, 200.f, 50.f};
    Color textColor{255, 255, 255, 255};
    
    SDLWindow window;
    window.create(800, 600, "Text Rendering Test");
    REQUIRE(window.get() != nullptr);

    SDLRenderer renderer(window.get());
    REQUIRE(renderer.get() != nullptr);

    SDLFont font(fontPath);
    
    DrawFontCommand cmd;
    cmd.text = textStr;
    cmd.font = &font;
    cmd.dest = textRect;
    cmd.fontSize = 18;
    cmd.color = textColor;
    cmd.layer = 0;
    cmd.zIndex = 0;
    cmd.order = 0;

    REQUIRE_NOTHROW(font.getFontWithSize(cmd.fontSize));

    {
        REQUIRE_NOTHROW(renderer.clear());
        REQUIRE_NOTHROW(renderer.draw(cmd));
        REQUIRE_NOTHROW(renderer.present());
    }

    TTF_Quit();
    SDL_Quit();
}

TEST_CASE_METHOD(SDLRendererFixture, "SDLRenderer can set and reset scale without errors", "[integration][sdl_renderer]")
{
    this->configureVideoDriverForCi();
    REQUIRE(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDLWindow window;
    window.create(800, 600, "Scale Test");
    SDLRenderer renderer(window.get());

    REQUIRE_NOTHROW(renderer.setScale(Position{2.f, 2.f}));
    REQUIRE_NOTHROW(renderer.setScale(Position{1.f, 1.f}));

    REQUIRE_NOTHROW(renderer.clear());
    REQUIRE_NOTHROW(renderer.present());

    SDL_Quit();
}
