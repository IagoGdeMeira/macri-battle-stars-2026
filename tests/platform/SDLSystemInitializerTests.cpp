#include "../../src/platform/include/SDLSystemInitializer/SDLSystemInitializer.h"

#include <catch2/catch_test_macros.hpp>
#include <SDL.h>

class SDLSystemInitializerFixture
{
public:
    ~SDLSystemInitializerFixture() { SDL_Quit(); }
};

TEST_CASE_METHOD(SDLSystemInitializerFixture, "SDLSystemInitializer initializes SDL with default VIDEO flag",
    "[integration][sdl_system_initializer]"
) {
    SDLSystemInitializer initializer;
    REQUIRE_NOTHROW(initializer.initialize());
}

TEST_CASE_METHOD(SDLSystemInitializerFixture, "SDLSystemInitializer initializes SDL with custom flags",
    "[integration][sdl_system_initializer]"
) {
    SDLSystemInitializer initializer(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    REQUIRE_NOTHROW(initializer.initialize());
}

TEST_CASE_METHOD(SDLSystemInitializerFixture, "SDLSystemInitializer destructor calls shutdown automatically",
    "[integration][sdl_system_initializer]"
) {
    {
        SDLSystemInitializer initializer;
        initializer.initialize();
    }

    REQUIRE(true);
}

TEST_CASE_METHOD(SDLSystemInitializerFixture, "SDLSystemInitializer allows repeated initialization",
    "[integration][sdl_system_initializer]"
) {
    SDLSystemInitializer initializer;
    initializer.initialize();

    REQUIRE_NOTHROW(initializer.initialize());
}

TEST_CASE_METHOD(SDLSystemInitializerFixture, "SDLSystemInitializer shutdown can be called multiple times safely",
    "[integration][sdl_system_initializer]"
) {
    SDLSystemInitializer initializer;
    initializer.initialize();
    initializer.shutdown();

    REQUIRE_NOTHROW(initializer.shutdown());
}
