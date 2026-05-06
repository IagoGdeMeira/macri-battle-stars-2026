#include "../../src/engine/include/TextureLoader/TextureLoader.h"

#include "../../src/engine/include/ITextureFactory/ITextureFactory.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>

class TextureLoaderFixture
{
public:
    struct StubTexture : public Texture
    {
        int getWidth() const override { return 0; }
        int getHeight() const override { return 0; }
    };
    struct StubFactory : ITextureFactory
    {
        int createTextureCalls = 0;
        std::string lastPath;
        std::shared_ptr<Texture> textureToReturn = std::make_shared<StubTexture>();

        std::shared_ptr<Texture> createTexture(const std::string& filePath) override
        {
            this->createTextureCalls++;
            this->lastPath = filePath;
            return this->textureToReturn;
        }
    };
};

TEST_CASE_METHOD(TextureLoaderFixture, "TextureLoader delegates to renderer and returns texture",
    "[unit][texture_loader]"
) {
    StubFactory factory;
    TextureLoader loader(factory);

    const auto texture = loader.load("assets/sprites/fighter_idle.png");

    REQUIRE(factory.createTextureCalls == 1);
    REQUIRE(texture == factory.textureToReturn);
}
