#include "../../src/platform/include/SDLRenderer/SDLRenderer.h"

#include "../../src/domain/include/Color/Color.h"

#include "../../src/engine/include/Viewport/Viewport.h"

#include "../../src/platform/include/SDLTexture/SDLTexture.h"
#include "../../src/platform/include/SDLWindow/SDLWindow.h"
#include "../../src/platform/include/SDLFont/SDLFont.h"

#include <catch2/catch_test_macros.hpp>
#include <SDL.h>
#include <SDL_ttf.h>
#include <fstream>

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
    REQUIRE_NOTHROW(renderer.setViewport(Viewport{ 0, 0, 800, 600 }));

    SDL_Quit();
}

TEST_CASE_METHOD(SDLRendererFixture, "SDLRenderer draws textures",
    "[unit][sdl_renderer]"
) {
    this->configureVideoDriverForCi();
    REQUIRE(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDLWindow window;
    window.create(800, 600, "Texture Rendering Test");

    SDLRenderer renderer(window.get());
    SDLTexture texture(nullptr);
    DrawTextureCommand cmd;
    cmd.texture = &texture;
    cmd.dest.position.x = 10.0f;
    cmd.dest.position.y = 20.0f;
    cmd.dest.size.width = 32.0f;
    cmd.dest.size.height = 48.0f;
    cmd.rotation = 30.0f;
    cmd.flipX = true;
    cmd.flipY = false;

    REQUIRE_NOTHROW(renderer.clear());
    REQUIRE_NOTHROW(renderer.drawTexture(cmd));
    REQUIRE_NOTHROW(renderer.present());

    SDL_Quit();
}

TEST_CASE_METHOD(SDLRendererFixture, "SDLRenderer draws rectangles",
    "[unit][sdl_renderer]"
) {
    this->configureVideoDriverForCi();
    REQUIRE(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDLWindow window;
    window.create(800, 600, "Rectangle Rendering Test");

    SDLRenderer renderer(window.get());

    REQUIRE_NOTHROW(renderer.clear());
    REQUIRE_NOTHROW(renderer.drawRectangle(DrawRectangleCommand{
        Rectangle{ { 0.0f, 0.0f }, 10.0f, 10.0f }, Color{} }));
    REQUIRE_NOTHROW(renderer.drawRectangle(DrawRectangleCommand{
        Rectangle{ { 1.0f, 1.0f }, 12.0f, 14.0f }, Color{ 10, 20, 30, 40 }, true }));
    REQUIRE_NOTHROW(renderer.present());

    SDL_Quit();
}

TEST_CASE_METHOD(SDLRendererFixture, "SDLRenderer draws circles",
    "[unit][sdl_renderer]"
) {
    this->configureVideoDriverForCi();
    REQUIRE(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDLWindow window;
    window.create(800, 600, "Circle Rendering Test");

    SDLRenderer renderer(window.get());

    REQUIRE_NOTHROW(renderer.clear());
    REQUIRE_NOTHROW(renderer.drawCircle(DrawCircleCommand{
        Circle{ { 4.0f, 5.0f }, 6.0f }, Color{} }));
    REQUIRE_NOTHROW(renderer.drawCircle(DrawCircleCommand{
        Circle{ { 7.0f, 8.0f }, 9.0f }, Color{ 10, 20, 30, 40 }, true }));
    REQUIRE_NOTHROW(renderer.present());

    SDL_Quit();
}

TEST_CASE_METHOD(SDLRendererFixture, "SDLRenderer has drawFont method defined",
    "[unit][sdl_renderer]"
) {
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
    Rectangle textRect{ { 50.0f, 50.0f }, 200.0f, 50.0f };
    Color textColor{ 255, 255, 255, 255 };
    
    SDLWindow window;
    window.create(800, 600, "Text Rendering Test");
    REQUIRE(window.get() != nullptr);

    SDLRenderer renderer(window.get());
    REQUIRE(renderer.get() != nullptr);

    SDLFont font(fontPath);
    
    DrawFontCommand cmd{ textStr, &font, textRect, 18, textColor };
    REQUIRE_NOTHROW(font.getFontWithSize(cmd.fontSize));

    {
        REQUIRE_NOTHROW(renderer.clear());
        REQUIRE_NOTHROW(renderer.drawFont(cmd));
        REQUIRE_NOTHROW(renderer.present());
    }

    TTF_Quit();
    SDL_Quit();
}
