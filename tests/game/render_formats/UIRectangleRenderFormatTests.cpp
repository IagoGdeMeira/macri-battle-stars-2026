#include "../../src/game/render_formats/UIRectangleRenderFormat.h"

#include "../../src/domain/components/RenderComponent.h"
#include "../../src/domain/components/UITransform.h"
#include "../../src/domain/components/VisualEffectsComponent.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/RenderContext/RenderContext.h"
#include "../../src/engine/include/Renderer/Renderer.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <vector>

class UIRectangleRenderFormatFixture
{
public:
    class StubRenderer : public Renderer
    {
    public:
        void clear() override {}
        void present() override {}

        void drawTexture(const DrawTextureCommand&) override {}
        void drawFont(const DrawFontCommand&) override {}

        void drawRectangle(const DrawRectangleCommand& cmd) override
        { this->rectangleCalls.push_back(cmd); }

        void drawCircle(const DrawCircleCommand&) override {}
        void setViewport(const Viewport&) override {}

        std::vector<DrawRectangleCommand> rectangleCalls;
    };

    UIRectangleRenderFormatFixture() : format(this->renderer), context { this->world, this->bus }
    {
        auto& components = this->world.components();
        components.registerComponent<UITransform>();
        components.registerComponent<RenderComponent>();
        components.registerComponent<VisualEffectsComponent>();
    }

    World world;
    EventBus bus;
    StubRenderer renderer;
    UIRectangleRenderFormat format;
    RenderContext context;
};

TEST_CASE_METHOD(UIRectangleRenderFormatFixture, "UIRectangleRenderFormat submits base and visual effect commands",
    "[unit][rectangle_render_format]"
) {
    Entity entity = this->world.entities().create();

    UITransform transform;
    transform.rect = Rectangle { Position { 15.0f, 25.0f }, Dimension2D { 40.0f, 12.0f } };

    this->world.components().add<UITransform>(entity, transform);
    this->world.components().add<RenderComponent>(entity, RenderComponent { 3, 9 });

    VisualEffectsComponent fx;
    fx.rectangleEffects.push_back([](DrawRectangleBatch& batch, DrawRectangleCommand& cmd) {
        DrawRectangleCommand glow = cmd;
        glow.filled = true;
        glow.color = Color { 9, 9, 9, 255 };
        batch.add(glow);
    });
    this->world.components().add<VisualEffectsComponent>(entity, fx);

    this->format.render(this->context);

    REQUIRE(this->renderer.rectangleCalls.size() == 2);

    const auto& effectCmd = this->renderer.rectangleCalls[0];
    REQUIRE(effectCmd.filled == true);
    REQUIRE(effectCmd.color == Color { 9, 9, 9, 255 });

    const auto& baseCmd = this->renderer.rectangleCalls[1];
    REQUIRE(baseCmd.rect.position.x == Catch::Approx(15.0f));
    REQUIRE(baseCmd.rect.position.y == Catch::Approx(25.0f));
    REQUIRE(baseCmd.rect.size.width == Catch::Approx(40.0f));
    REQUIRE(baseCmd.rect.size.height == Catch::Approx(12.0f));
    REQUIRE(baseCmd.filled == false);
    REQUIRE(baseCmd.color == Color::WHITE());
    REQUIRE(baseCmd.layer == 3);
    REQUIRE(baseCmd.zIndex == 9);
    REQUIRE(baseCmd.order == 0);
}
