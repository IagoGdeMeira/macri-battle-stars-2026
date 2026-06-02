#include "../../src/game/render_formats/UIFontRenderFormat.h"

#include "../../src/domain/components/RenderComponent.h"
#include "../../src/domain/components/UITextComponent.h"
#include "../../src/domain/components/UITransform.h"
#include "../../src/domain/components/VisualEffectsComponent.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/RenderContext/RenderContext.h"
#include "../../src/engine/include/Renderer/Renderer.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <vector>

class UIFontRenderFormatFixture
{
public:
    class StubFont : public Font
    {
    public:
        int getAscent(int) const override { return 8; }
        int getDescent(int) const override { return 2; }
        int getHeight(int) const override { return 10; }
    };

    class StubRenderer : public Renderer
    {
    public:
        void clear() override {}
        void present() override {}

        void drawTexture(const DrawTextureCommand&) override {}

        void drawFont(const DrawFontCommand& cmd) override
        { this->fontCalls.push_back(cmd); }

        void drawRectangle(const DrawRectangleCommand&) override {}
        void drawCircle(const DrawCircleCommand&) override {}
        void setViewport(const Viewport&) override {}

        std::vector<DrawFontCommand> fontCalls;
    };

    UIFontRenderFormatFixture() : format(this->renderer), context { this->world, this->bus }
    {
        auto& components = this->world.components();
        components.registerComponent<RenderComponent>();
        components.registerComponent<UITextComponent>();
        components.registerComponent<UITransform>();
        components.registerComponent<VisualEffectsComponent>();
    }

    World world;
    EventBus bus;
    StubRenderer renderer;
    UIFontRenderFormat format;
    RenderContext context;
};

TEST_CASE_METHOD(UIFontRenderFormatFixture, "UIFontRenderFormat submits base and visual effect commands",
    "[unit][font_render_format]"
) {
    auto font = std::make_shared<StubFont>();

    Entity entity = this->world.entities().create();

    UITransform transform;
    transform.rect = Rectangle { Position { 30.0f, 50.0f }, Dimension2D { 80.0f, 24.0f } };

    UITextComponent text;
    text.font = font;
    text.text = "Play";
    text.color = Color { 5, 6, 7, 8 };
    text.fontSize = 0.0f;

    this->world.components().add<UITransform>(entity, transform);
    this->world.components().add<UITextComponent>(entity, text);
    this->world.components().add<RenderComponent>(entity, RenderComponent { 4, 1 });

    VisualEffectsComponent fx;
    fx.fontEffects.push_back([](DrawFontBatch& batch, DrawFontCommand& cmd) {
        DrawFontCommand outline = cmd;
        outline.color = Color { 1, 2, 3, 4 };
        outline.dest.position.y += 2.0f;
        batch.add(outline);
    });
    this->world.components().add<VisualEffectsComponent>(entity, fx);

    this->format.render(this->context);

    REQUIRE(this->renderer.fontCalls.size() == 2);

    const auto& effectCmd = this->renderer.fontCalls[0];
    REQUIRE(effectCmd.color == Color { 1, 2, 3, 4 });
    REQUIRE(effectCmd.dest.position.y == Catch::Approx(52.0f));

    const auto& baseCmd = this->renderer.fontCalls[1];
    REQUIRE(baseCmd.font == font.get());
    REQUIRE(baseCmd.text == "Play");
    REQUIRE(baseCmd.dest.position.x == Catch::Approx(30.0f));
    REQUIRE(baseCmd.dest.position.y == Catch::Approx(50.0f));
    REQUIRE(baseCmd.fontSize == 16);
    REQUIRE(baseCmd.color == Color { 5, 6, 7, 8 });
    REQUIRE(baseCmd.layer == 4);
    REQUIRE(baseCmd.zIndex == 1);
    REQUIRE(baseCmd.order == 0);
}
