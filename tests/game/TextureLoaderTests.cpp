#include "../../src/game/include/TextureLoader/TextureLoader.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>

class TextureLoaderFixture
{
public:
    struct StubRenderer : Renderer
    {
        int clearCalls = 0;
        int presentCalls = 0;
        int drawCalls = 0;
        int viewportCalls = 0;
        int createTextureCalls = 0;
        std::string lastPath;
        std::shared_ptr<Texture> textureToReturn = std::make_shared<Texture>();

        void clear() override { this->clearCalls++; }
        void present() override { this->presentCalls++; }

        std::shared_ptr<Texture> createTexture(const std::string& filePath) override
        {
            this->createTextureCalls++;
            this->lastPath = filePath;
            return this->textureToReturn;
        }

        void drawTexture(const Texture& texture, const Renderer::DrawTextureParams& params) override
        { (void)texture; (void)params; this->drawCalls++; }

        void drawRectOutline(const Rectangle& rect, const Renderer::Color& color) override
        { (void)rect; (void)color; }

        void drawRectFilled(const Rectangle& rect, const Renderer::Color& color) override
        { (void)rect; (void)color; }

        void drawCircleOutline(const Circle& circle, const Renderer::Color& color) override
        { (void)circle; (void)color; }

        void drawCircleFilled(const Circle& circle, const Renderer::Color& color) override
        { (void)circle; (void)color; }

        void setViewport(const Viewport& viewport) override
        {
            (void)viewport;
            this->viewportCalls++;
        }
    };
};

TEST_CASE_METHOD(TextureLoaderFixture, "TextureLoader delegates to renderer and returns texture",
    "[unit][texture_loader]"
) {
    StubRenderer renderer;
    TextureLoader loader(renderer);

    const auto texture = loader.load("assets/sprites/fighter_idle.png");

    REQUIRE(renderer.createTextureCalls == 1);
    REQUIRE(renderer.lastPath == "assets/sprites/fighter_idle.png");
    REQUIRE(texture == renderer.textureToReturn);
}
