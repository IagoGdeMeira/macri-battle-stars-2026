#include "game/render_formats/UIFontRenderFormat.h"

#include "StubFont.h"
#include "StubRenderer.h"

#include "domain/components/RenderComponent.h"
#include "domain/components/UITextComponent.h"
#include "domain/components/UITransform.h"
#include "domain/components/VisualEffectsComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/Renderer/Renderer.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

class UIFontRenderFormatFixture
{
public:
    World world;
    EventBus bus;
    StubRenderer renderer;
    UIFontRenderFormat format;
    RenderContext context;

    UIFontRenderFormatFixture() : format(this->renderer), context { this->world, this->bus }
    {
        auto& comp = this->world.components();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<UITextComponent>();
        comp.registerComponent<UITransform>();
        comp.registerComponent<VisualEffectsComponent>();
    }
};

TEST_CASE_METHOD(UIFontRenderFormatFixture, "UIFontRenderFormat submits base and visual effect commands",
    "[unit][font_render_format]"
) {
    auto font = std::make_shared<StubFont>();

    Entity entity = this->world.entities().create();

    UITransform transform;
    transform.rect = Rectangle{Position{30.f, 50.f}, Dimension2D{80.f, 24.f}};

    UITextComponent text;
    text.font = font;
    text.text = "Play";
    text.color = Color { 5, 6, 7, 8 };
    text.fontSize = 0.f;

    auto& comp = this->world.components();
    comp.add<UITransform>(entity, transform);
    comp.add<UITextComponent>(entity, text);
    comp.add<RenderComponent>(entity, RenderComponent { 4, 1 });

    VisualEffectsComponent fx;
    fx.fontEffects.push_back([](DrawFontBatch& batch, DrawFontCommand& cmd) {
        DrawFontCommand outline = cmd;
        outline.color = Color {1, 2, 3, 4};
        outline.dest.position.y += 2.f;
        batch.add(outline);
    });
    comp.add<VisualEffectsComponent>(entity, fx);

    this->format.render(this->context);

    REQUIRE(this->renderer.fontCalls.size() == 2);

    const auto& effectCmd = this->renderer.fontCalls[0];
    REQUIRE(effectCmd.color == Color { 1, 2, 3, 4 });
    REQUIRE(effectCmd.dest.position.y == Catch::Approx(52.f));

    const auto& baseCmd = this->renderer.fontCalls[1];
    REQUIRE(baseCmd.font == font.get());
    REQUIRE(baseCmd.text == "Play");
    REQUIRE(baseCmd.dest.position.x == Catch::Approx(30.f));
    REQUIRE(baseCmd.dest.position.y == Catch::Approx(50.f));
    REQUIRE(baseCmd.fontSize == 16);
    REQUIRE(baseCmd.color == Color {5, 6, 7, 8});
    REQUIRE(baseCmd.layer == 4);
    REQUIRE(baseCmd.zIndex == 1);
    REQUIRE(baseCmd.order == 0);
}
