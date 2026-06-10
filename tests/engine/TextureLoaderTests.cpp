#include "../../src/engine/include/TextureLoader/TextureLoader.h"

#include "../stubs/StubTextureFactory.h"
#include "../stubs/StubTexture.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>

TEST_CASE("TextureLoader delegates to renderer and returns texture", "[unit][texture_loader]")
{
    StubTextureFactory factory;
    TextureLoader loader(factory);

    const auto texture = loader.load("assets/sprites/fighter_idle.png");

    REQUIRE(factory.createTextureCalls == 1);
    REQUIRE(texture == factory.textureToReturn);
}
