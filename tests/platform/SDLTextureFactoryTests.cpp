#include "platform/include/SDLTextureFactory/SDLTextureFactory.h"

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <SDL.h>
#include <string>

class SDLTextureFactoryFixture
{
public:
    std::filesystem::path makeTempBmpPath(const char* suffix) const
    {
        const auto ticks = SDL_GetTicks();
        return std::filesystem::temp_directory_path() / (std::string("mbs_texture_factory_")
            + suffix + "_" + std::to_string(ticks) + ".bmp");
    }

    void configureVideoDriverForCi() const
    {
        #if defined(__linux__)
            SDL_setenv("SDL_VIDEODRIVER", "dummy", 1);
            SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
        #endif
    }
};

TEST_CASE_METHOD(SDLTextureFactoryFixture, "SDLTextureFactory returns nullptr for invalid path",
    "[integration][sdl_texture_factory]"
) {
    this->configureVideoDriverForCi();
    REQUIRE(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDL_Window* window = SDL_CreateWindow(
        "TextureFactoryTest", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 64, 64, SDL_WINDOW_SHOWN);
    REQUIRE(window != nullptr);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    REQUIRE(renderer != nullptr);

    SDLTextureFactory factory(renderer);
    const auto texture = factory.createTexture("this/path/does/not/exist.png");

    REQUIRE(texture == nullptr);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

TEST_CASE_METHOD(SDLTextureFactoryFixture, "SDLTextureFactory loads a valid bmp file",
    "[integration][sdl_texture_factory]"
) {
    this->configureVideoDriverForCi();
    REQUIRE(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDL_Window* window = SDL_CreateWindow(
        "TextureFactoryTest", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 64, 64, SDL_WINDOW_SHOWN);
    REQUIRE(window != nullptr);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    REQUIRE(renderer != nullptr);

    const auto bmpPath = this->makeTempBmpPath("ok");

    SDL_Surface* surface = SDL_CreateRGBSurface(0, 32, 16, 32, 0, 0, 0, 0);
    REQUIRE(surface != nullptr);
    REQUIRE(SDL_SaveBMP(surface, bmpPath.string().c_str()) == 0);
    SDL_FreeSurface(surface);

    SDLTextureFactory factory(renderer);
    const auto texture = factory.createTexture(bmpPath.string());

    REQUIRE(texture != nullptr);
    REQUIRE(texture->getWidth() == 32);
    REQUIRE(texture->getHeight() == 16);

    std::error_code ec;
    std::filesystem::remove(bmpPath, ec);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
