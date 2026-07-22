#include "game/render_formats/UITextureRenderFormat.h"

#include "StubRenderer.h"
#include "StubTexture.h"

#include "domain/components/RenderComponent.h"
#include "domain/components/UISpriteComponent.h"
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

class UITextureRenderFormatFixture
{
public:
    World world;
    EventBus bus;
    StubRenderer renderer;
    UITextureRenderFormat format;
    RenderContext context;

    UITextureRenderFormatFixture() : format(this->renderer), context { this->world, this->bus }
    {
        auto& comp = this->world.components();
        comp.registerComponent<UITransform>();
        comp.registerComponent<UISpriteComponent>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<VisualEffectsComponent>();
    }
};

TEST_CASE_METHOD(UITextureRenderFormatFixture, "UITextureRenderFormat submits base and visual effect commands",
    "[unit][texture_render_format]"
) {
    auto texture = std::make_shared<StubTexture>();

    Entity entity = this->world.entities().create();

    UITransform transform;
    transform.rect = Rectangle{Position{10.f, 20.f}, Dimension2D{100.f, 30.f}};
    transform.rotation = 45.f;
    transform.scale = Position {-1.f, 2.f};

    auto& comp = this->world.components();
    comp.add<UITransform>(entity, transform);
    comp.add<UISpriteComponent>(entity, UISpriteComponent { texture, Color { 10, 20, 30, 40 } });
    comp.add<RenderComponent>(entity, RenderComponent { 2, 3 });

    VisualEffectsComponent fx;
    fx.textureEffects.push_back([](DrawTextureBatch& batch, DrawTextureCommand& cmd) {
        DrawTextureCommand shadow = cmd;
        shadow.tint = Color { 200, 10, 10, 255 };
        shadow.dest.position.x += 1.f;
        batch.add(shadow);
    });
    comp.add<VisualEffectsComponent>(entity, fx);

    this->format.render(this->context);

    REQUIRE(this->renderer.textureCalls.size() == 2);

    const auto& effectCmd = this->renderer.textureCalls[0];
    REQUIRE(effectCmd.tint == Color { 200, 10, 10, 255 });
    REQUIRE(effectCmd.dest.position.x == Catch::Approx(11.f));

    const auto& baseCmd = this->renderer.textureCalls[1];
    REQUIRE(baseCmd.texture == texture);
    REQUIRE(baseCmd.dest.position.x == Catch::Approx(10.f));
    REQUIRE(baseCmd.dest.position.y == Catch::Approx(20.f));
    REQUIRE(baseCmd.rotation == Catch::Approx(45.f));
    REQUIRE(baseCmd.flipX == true);
    REQUIRE(baseCmd.flipY == false);
    REQUIRE(baseCmd.layer == 2);
    REQUIRE(baseCmd.zIndex == 3);
    REQUIRE(baseCmd.order == 0);
    REQUIRE(baseCmd.blend == BlendMode::Normal);
}
