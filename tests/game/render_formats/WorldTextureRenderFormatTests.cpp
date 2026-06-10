#include "../../../src/game/render_formats/WorldTextureRenderFormat.h"

#include "../../stubs/StubRenderer.h"
#include "../../stubs/StubTexture.h"

#include "../../../src/domain/components/AnimationControllerComponent.h"
#include "../../../src/domain/components/OrientationComponent.h"
#include "../../../src/domain/components/ParallaxComponent.h"
#include "../../../src/domain/components/RenderComponent.h"
#include "../../../src/domain/components/SpriteComponent.h"
#include "../../../src/domain/components/TransformComponent.h"
#include "../../../src/domain/components/VisualEffectsComponent.h"
#include "../../../src/domain/include/World/World.h"

#include "../../../src/engine/include/EventBus/EventBus.h"
#include "../../../src/engine/include/RenderContext/RenderContext.h"
#include "../../../src/engine/include/Renderer/Renderer.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

class WorldTextureRenderFormatFixture
{
public:
    WorldTextureRenderFormatFixture() : format(this->renderer, this->camera), context { this->world, this->bus }
    {
        auto& components = this->world.components();
        components.registerComponent<SpriteComponent>();
        components.registerComponent<TransformComponent>();
        components.registerComponent<RenderComponent>();
        components.registerComponent<OrientationComponent>();
        components.registerComponent<AnimationControllerComponent>();
        components.registerComponent<ParallaxComponent>();
        components.registerComponent<VisualEffectsComponent>();

        this->camera.setPosition(50.0f, 20.0f);
        this->camera.setZoom(2.0f);
        this->format.setViewport(Viewport { 0, 0, 800, 600 });
    }

    World world;
    EventBus bus;
    StubRenderer renderer;
    Camera2D camera;
    WorldTextureRenderFormat format;
    RenderContext context;
};

TEST_CASE_METHOD(WorldTextureRenderFormatFixture, "WorldTextureRenderFormat submits base and visual effect commands",
    "[unit][world_texture_render_format]"
) {
    const auto texture = std::make_shared<StubTexture>();
    const Entity entity = this->world.entities().create();

    this->world.components().add<TransformComponent>(entity,
        TransformComponent {Position {100.f, 80.f}, Position {-1.5f, 2.f}, 30.f});
    this->world.components().add<SpriteComponent>(entity,
        SpriteComponent {texture, Dimension2D {20.f, 10.f}, Rectangle {Position{2.f, 3.f}, Dimension2D{4.f, 5.f}}, true });
    this->world.components().add<RenderComponent>(entity, RenderComponent { 7, 9 });
    this->world.components().add<ParallaxComponent>(entity, ParallaxComponent{Position{ 0.5f, 1.f }});
    this->world.components().add<OrientationComponent>(entity, OrientationComponent { Orientation::Left });

    AnimationControllerComponent animationController;
    animationController.animations.symmetric = true;
    this->world.components().add<AnimationControllerComponent>(entity, animationController);

    VisualEffectsComponent fx;
    fx.textureEffects.push_back([](DrawTextureBatch& batch, DrawTextureCommand& cmd) 
    {
        DrawTextureCommand echo = cmd;
        echo.tint = Color { 1, 2, 3, 255 };
        echo.dest.position.x += 3.f;
        batch.add(echo);
    });
    this->world.components().add<VisualEffectsComponent>(entity, fx);

    this->format.render(this->context);

    REQUIRE(this->renderer.textureCalls.size() == 2);

    const auto& effectCmd = this->renderer.textureCalls[0];
    REQUIRE(effectCmd.tint == Color { 1, 2, 3, 255 });
    REQUIRE(effectCmd.dest.position.x == Catch::Approx(553.f));

    const auto& baseCmd = this->renderer.textureCalls[1];
    REQUIRE(baseCmd.texture == texture.get());
    REQUIRE(baseCmd.dest.position.x == Catch::Approx(550.f));
    REQUIRE(baseCmd.dest.position.y == Catch::Approx(420.f));
    REQUIRE(baseCmd.dest.size.width == Catch::Approx(60.f));
    REQUIRE(baseCmd.dest.size.height == Catch::Approx(40.f));
    REQUIRE(baseCmd.rotation == Catch::Approx(30.f));
    REQUIRE(baseCmd.flipX == true);
    REQUIRE(baseCmd.flipY == false);
    REQUIRE(baseCmd.layer == 7);
    REQUIRE(baseCmd.zIndex == 9);
    REQUIRE(baseCmd.order == 0);
    REQUIRE(baseCmd.source.position.x == Catch::Approx(2.f));
    REQUIRE(baseCmd.source.position.y == Catch::Approx(3.f));
    REQUIRE(baseCmd.source.size.width == Catch::Approx(4.f));
    REQUIRE(baseCmd.source.size.height == Catch::Approx(5.f));
    REQUIRE(baseCmd.useSourceRect == true);
}

TEST_CASE_METHOD(WorldTextureRenderFormatFixture, "WorldTextureRenderFormat skips sprites without texture",
    "[unit][world_texture_render_format]"
) {
    const Entity entity = this->world.entities().create();

    this->world.components().add<TransformComponent>(entity,
        TransformComponent{Position{10.f, 20.f}, Position{1.f, 1.f}, 0.f});
    this->world.components().add<SpriteComponent>(entity,
        SpriteComponent { nullptr, Dimension2D { 20.f, 10.f }, Rectangle {}, false });
    this->world.components().add<RenderComponent>(entity, RenderComponent { 0, 0 });

    this->format.render(this->context);

    REQUIRE(this->renderer.textureCalls.empty());
}

TEST_CASE_METHOD(WorldTextureRenderFormatFixture, "WorldTextureRenderFormat respects non-symmetric animation orientation",
    "[unit][world_texture_render_format]"
) {
    const auto texture = std::make_shared<StubTexture>();
    const Entity entity = this->world.entities().create();

    this->world.components().add<TransformComponent>(entity,
        TransformComponent{Position{100.f, 80.f}, Position{-1.f, 1.f}, 0.f});
    this->world.components().add<SpriteComponent>(entity,
        SpriteComponent { texture, Dimension2D { 20.f, 10.f }, Rectangle {}, false });
    this->world.components().add<RenderComponent>(entity, RenderComponent { 0, 0 });
    this->world.components().add<OrientationComponent>(entity, OrientationComponent { Orientation::Left });

    AnimationControllerComponent animationController;
    animationController.animations.symmetric = false;
    this->world.components().add<AnimationControllerComponent>(entity, animationController);

    this->format.render(this->context);

    REQUIRE(this->renderer.textureCalls.size() == 1);
    REQUIRE(this->renderer.textureCalls[0].flipX == false);
}
