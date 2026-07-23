#include "platform/draw_commands/SDLDrawTextureCommandHandler.h"

#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include "platform/include/SDLWindow/SDLWindow.h"
#include "platform/include/SDLTexture/SDLTexture.h"

#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <memory>
#include <SDL.h>

class SDLDrawTextureCommandHandlerFixture
{
public:
    SDLDrawTextureCommandHandlerFixture()
    {
        #if defined(__linux__)
            SDL_setenv("SDL_VIDEODRIVER", "dummy", 1);
            SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
        #endif

        REQUIRE(SDL_Init(SDL_INIT_VIDEO) == 0);

        this->window.create(800, 600, "Texture Handler Test");
        this->renderer = SDL_CreateRenderer(this->window.get(), -1, SDL_RENDERER_SOFTWARE);
        REQUIRE(this->renderer != nullptr);
    }

    ~SDLDrawTextureCommandHandlerFixture()
    {
        if (this->renderer) SDL_DestroyRenderer(this->renderer);
        SDL_Quit();
    }

    SDL_Renderer* getRenderer() const { return this->renderer; }

    std::shared_ptr<SDLTexture> createTestTexture()
    {
        SDL_Surface* surface = SDL_CreateRGBSurface(0, 1, 1, 32, 0, 0, 0, 0);
        if (!surface) return nullptr;

        SDL_Texture* tex = SDL_CreateTextureFromSurface(this->renderer, surface);
        SDL_FreeSurface(surface);
        if (!tex) return nullptr;

        return std::make_shared<SDLTexture>(tex);
    }

private:
    SDLWindow window;
    SDL_Renderer* renderer = nullptr;
};

TEST_CASE_METHOD(SDLDrawTextureCommandHandlerFixture, "SDLDrawTextureCommandHandler handles null texture gracefully",
    "[unit][sdl_draw_texture_command_handler]"
) {
    SDLDrawTextureCommandHandler handler(this->getRenderer());

    DrawTextureCommand cmd;
    cmd.texture = nullptr;
    cmd.dest = Rectangle{Position{10.f, 20.f}, Dimension2D{32.f, 48.f}};

    SDL_ClearError();
    REQUIRE_NOTHROW(handler.execute(cmd));

    const char* error = SDL_GetError();
    REQUIRE(std::strlen(error) == 0);
}

TEST_CASE_METHOD(SDLDrawTextureCommandHandlerFixture, "SDLDrawTextureCommandHandler draws with valid texture",
    "[unit][sdl_draw_texture_command_handler]"
) {
    auto texture = this->createTestTexture();
    REQUIRE(texture != nullptr);

    SDLDrawTextureCommandHandler handler(this->getRenderer());

    DrawTextureCommand cmd;
    cmd.texture = texture;
    cmd.dest = Rectangle{Position{100.f, 100.f}, Dimension2D{64.f, 64.f}};
    cmd.source = Rectangle{Position{0.f, 0.f}, Dimension2D{64.f, 64.f}};
    cmd.useSourceRect = true;
    cmd.rotation = 45.f;
    cmd.pivot = Position{0.5f, 0.5f};
    cmd.flipX = true;
    cmd.flipY = false;
    cmd.tint = Color{255, 0, 0, 255};
    cmd.blend = BlendMode::Normal;

    SDL_ClearError();
    REQUIRE_NOTHROW(handler.execute(cmd));

    const char* error = SDL_GetError();
    REQUIRE(std::strlen(error) == 0);
}

TEST_CASE_METHOD(SDLDrawTextureCommandHandlerFixture, "SDLDrawTextureCommandHandler handles all blend modes",
    "[unit][sdl_draw_texture_command_handler]"
) {
    auto texture = this->createTestTexture();
    REQUIRE(texture != nullptr);

    SDLDrawTextureCommandHandler handler(this->getRenderer());

    std::vector<BlendMode> modes = {BlendMode::Normal, BlendMode::Add, BlendMode::Multiply};
    for (auto mode : modes)
    {
        DrawTextureCommand cmd;
        cmd.texture = texture;
        cmd.dest = Rectangle{Position{0.f, 0.f}, Dimension2D{32.f, 32.f}};
        cmd.blend = mode;

        SDL_ClearError();
        REQUIRE_NOTHROW(handler.execute(cmd));

        const char* error = SDL_GetError();
        REQUIRE(std::strlen(error) == 0);
    }
}
