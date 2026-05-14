#include "../../src/platform/include/SDLFont/SDLFont.h"

#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <SDL_ttf.h>
#include <stdexcept>

class SDLFontFixture
{
public:
    SDLFontFixture()
    { if (TTF_Init() != 0) throw std::runtime_error(std::string("TTF_Init failed: ") + SDL_GetError()); }

    ~SDLFontFixture() { TTF_Quit(); }
};

TEST_CASE_METHOD(SDLFontFixture, "SDLFont throws on invalid font path",
    "[unit][sdl_font]"
) {
    REQUIRE_THROWS_AS(SDLFont("/nonexistent/path/to/font.ttf"), std::runtime_error);
}

TEST_CASE_METHOD(SDLFontFixture, "SDLFont retrieves ascent metric at default size",
    "[integration][sdl_font]"
) {
    #if defined(_WIN32)
        const std::string fontPath = "C:\\Windows\\Fonts\\arial.ttf";
    #else
        const std::string fontPath = "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf";
    #endif

    std::ifstream fontFile(fontPath);
    if (!fontFile.good()) SKIP("Font file not available at " + fontPath);

    SDLFont font(fontPath);

    int ascent = font.getAscent(24);
    REQUIRE(ascent > 0);
}

TEST_CASE_METHOD(SDLFontFixture, "SDLFont retrieves descent metric at different sizes",
    "[integration][sdl_font]"
) {
    #if defined(_WIN32)
        const std::string fontPath = "C:\\Windows\\Fonts\\arial.ttf";
    #else
        const std::string fontPath = "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf";
    #endif

    std::ifstream fontFile(fontPath);
    if (!fontFile.good()) SKIP("Font file not available at " + fontPath);

    SDLFont font(fontPath);

    int descent12 = font.getDescent(12);
    int descent24 = font.getDescent(24);
    int descent48 = font.getDescent(48);

    REQUIRE(descent12 >= 0);
    REQUIRE(descent24 >= 0);
    REQUIRE(descent48 >= 0);
}

TEST_CASE_METHOD(SDLFontFixture, "SDLFont retrieves height metric at different sizes",
    "[integration][sdl_font]"
) {
    #if defined(_WIN32)
        const std::string fontPath = "C:\\Windows\\Fonts\\arial.ttf";
    #else
        const std::string fontPath = "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf";
    #endif

    std::ifstream fontFile(fontPath);
    if (!fontFile.good()) SKIP("Font file not available at " + fontPath);
    
    SDLFont font(fontPath);

    int height12 = font.getHeight(12);
    int height24 = font.getHeight(24);
    int height48 = font.getHeight(48);

    REQUIRE(height12 > 0);
    REQUIRE(height24 > 0);
    REQUIRE(height48 > 0);
    
    REQUIRE(height24 > height12);
    REQUIRE(height48 > height24);
}

TEST_CASE_METHOD(SDLFontFixture, "SDLFont caches fonts at different sizes",
    "[integration][sdl_font]"
) {
    #if defined(_WIN32)
        const std::string fontPath = "C:\\Windows\\Fonts\\arial.ttf";
    #else
        const std::string fontPath = "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf";
    #endif

    std::ifstream fontFile(fontPath);
    if (!fontFile.good()) SKIP("Font file not available at " + fontPath);

    SDLFont font(fontPath);

    int firstHeight24 = font.getHeight(24);
    int secondHeight24 = font.getHeight(24);

    REQUIRE(firstHeight24 == secondHeight24);

    int firstHeight32 = font.getHeight(32);
    REQUIRE(firstHeight32 > 0);
}

TEST_CASE_METHOD(SDLFontFixture, "SDLFont metrics are consistent across multiple queries",
    "[integration][sdl_font]"
) {
    #if defined(_WIN32)
        const std::string fontPath = "C:\\Windows\\Fonts\\arial.ttf";
    #else
        const std::string fontPath = "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf";
    #endif

    std::ifstream fontFile(fontPath);
    if (!fontFile.good()) SKIP("Font file not available at " + fontPath);

    SDLFont font(fontPath);

    int size = 24;

    int ascent1 = font.getAscent(size);
    int ascent2 = font.getAscent(size);
    REQUIRE(ascent1 == ascent2);

    int descent1 = font.getDescent(size);
    int descent2 = font.getDescent(size);
    REQUIRE(descent1 == descent2);

    int height1 = font.getHeight(size);
    int height2 = font.getHeight(size);
    REQUIRE(height1 == height2);
}
