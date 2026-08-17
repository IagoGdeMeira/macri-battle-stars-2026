#include "game/render_formats/UIFontRenderFormat.h"

#include "StubFont.h"
#include "StubRenderer.h"

#include "domain/components/FontEffectsComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/components/UITextComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/RenderQueue/RenderQueue.h"
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
        comp.registerComponent<FontEffectsComponent>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<UITextComponent>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<UILayoutMetricsComponent>();
    }
};

TEST_CASE_METHOD(UIFontRenderFormatFixture, "UIFontRenderFormat submits base and visual effect commands",
    "[unit][font_render_format]"
) {
    auto font = std::make_shared<StubFont>();

    Entity entity = this->world.entities().create();

    TransformComponent tc;
    tc.position = {30.f, 50.f};

    UILayoutMetricsComponent layout;
    layout.size = {80.f, 24.f};

    UITextComponent text;
    text.font = font;
    text.text = "Play";
    text.color = Color { 5, 6, 7, 8 };
    text.fontSize = 0.f;

    auto& comp = this->world.components();
    comp.add<TransformComponent>(entity, tc);
    comp.add<UILayoutMetricsComponent>(entity, layout);
    comp.add<UITextComponent>(entity, text);
    comp.add<RenderComponent>(entity, RenderComponent { 4, 1 });

    FontEffectsComponent fx;
    fx.effects.push_back([](void* q, void* c) {
        auto& queue = *static_cast<RenderQueue*>(q);
        auto& cmd = *static_cast<DrawFontCommand*>(c);
        auto outline = std::make_unique<DrawFontCommand>(cmd);
        outline->color = Color {1, 2, 3, 4};
        outline->dest.position.y += 2.f;
        queue.add(std::move(outline));
    });
    comp.add<FontEffectsComponent>(entity, fx);

    RenderQueue queue;
    this->format.render(this->context, queue);
    queue.submit(this->renderer);

    REQUIRE(this->renderer.fontCalls.size() == 2);

    const auto& baseCmd = this->renderer.fontCalls[0];
    REQUIRE(baseCmd.font == font.get());
    REQUIRE(baseCmd.text == "Play");
    REQUIRE(baseCmd.dest.position.x == Catch::Approx(30.f));
    REQUIRE(baseCmd.dest.position.y == Catch::Approx(50.f));
    REQUIRE(baseCmd.fontSize == 16);
    REQUIRE(baseCmd.color == Color {5, 6, 7, 8});
    REQUIRE(baseCmd.layer == 4);
    REQUIRE(baseCmd.zIndex == 1);
    REQUIRE(baseCmd.order == 0);

    const auto& effectCmd = this->renderer.fontCalls[1];
    REQUIRE(effectCmd.color == Color { 1, 2, 3, 4 });
    REQUIRE(effectCmd.dest.position.y == Catch::Approx(52.f));
}
