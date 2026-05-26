#include "../../../src/game/render_formats/TextureRenderFormat.h"

#include "../../../src/domain/components/RenderComponent.h"
#include "../../../src/domain/components/UISpriteComponent.h"
#include "../../../src/domain/components/UITransform.h"
#include "../../../src/domain/components/VisualEffectsComponent.h"
#include "../../../src/domain/include/World/World.h"

#include "../../../src/engine/include/EventBus/EventBus.h"
#include "../../../src/engine/include/RenderContext/RenderContext.h"
#include "../../../src/engine/include/Renderer/Renderer.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <vector>

class TextureRenderFormatFixture
{
public:
    class StubTexture : public Texture
    {
    public:
        int getWidth() const override { return 32; }
        int getHeight() const override { return 16; }
    };

    class StubRenderer : public Renderer
    {
    public:
        void clear() override {}
        void present() override {}

        void drawTexture(const DrawTextureCommand& cmd) override
        { this->textureCalls.push_back(cmd); }

        void drawFont(const DrawFontCommand&) override {}
        void drawRectangle(const DrawRectangleCommand&) override {}
        void drawCircle(const DrawCircleCommand&) override {}
        void setViewport(const Viewport&) override {}

        std::vector<DrawTextureCommand> textureCalls;
    };

    TextureRenderFormatFixture() : format(this->renderer), context { this->world, this->bus }
    {
        auto& components = this->world.components();
        components.registerComponent<UITransform>();
        components.registerComponent<UISpriteComponent>();
        components.registerComponent<RenderComponent>();
        components.registerComponent<VisualEffectsComponent>();
    }

    World world;
    EventBus bus;
    StubRenderer renderer;
    TextureRenderFormat format;
    RenderContext context;
};

TEST_CASE_METHOD(TextureRenderFormatFixture, "TextureRenderFormat submits base and visual effect commands",
    "[unit][texture_render_format]"
) {
    auto texture = std::make_shared<StubTexture>();

    Entity entity = this->world.entities().create();

    UITransform transform;
    transform.rect = Rectangle { Position { 10.0f, 20.0f }, Dimension2D { 100.0f, 30.0f } };
    transform.rotation = 45.0f;
    transform.scale = Position { -1.0f, 2.0f };

    this->world.components().add<UITransform>(entity, transform);
    this->world.components().add<UISpriteComponent>(entity,
        UISpriteComponent { texture, Color { 10, 20, 30, 40 } });
    this->world.components().add<RenderComponent>(entity, RenderComponent { 2, 3 });

    VisualEffectsComponent fx;
    fx.textureEffects.push_back([](DrawTextureBatch& batch, DrawTextureCommand& cmd) {
        DrawTextureCommand shadow = cmd;
        shadow.tint = Color { 200, 10, 10, 255 };
        shadow.dest.position.x += 1.0f;
        batch.add(shadow);
    });
    this->world.components().add<VisualEffectsComponent>(entity, fx);

    this->format.render(this->context);

    REQUIRE(this->renderer.textureCalls.size() == 2);

    const auto& effectCmd = this->renderer.textureCalls[0];
    REQUIRE(effectCmd.tint == Color { 200, 10, 10, 255 });
    REQUIRE(effectCmd.dest.position.x == Catch::Approx(11.0f));

    const auto& baseCmd = this->renderer.textureCalls[1];
    REQUIRE(baseCmd.texture == texture.get());
    REQUIRE(baseCmd.dest.position.x == Catch::Approx(10.0f));
    REQUIRE(baseCmd.dest.position.y == Catch::Approx(20.0f));
    REQUIRE(baseCmd.rotation == Catch::Approx(45.0f));
    REQUIRE(baseCmd.flipX == true);
    REQUIRE(baseCmd.flipY == false);
    REQUIRE(baseCmd.layer == 2);
    REQUIRE(baseCmd.zIndex == 3);
    REQUIRE(baseCmd.order == 0);
    REQUIRE(baseCmd.blend == BlendMode::Normal);
}
