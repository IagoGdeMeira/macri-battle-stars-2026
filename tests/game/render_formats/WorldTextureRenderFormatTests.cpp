#include "game/render_formats/WorldTextureRenderFormat.h"

#include "StubRenderer.h"
#include "StubTextureFactory.h"
#include "StubTextureLoader.h"

#include "domain/components/AnimationControllerComponent.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/ParallaxComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/VisualEffectsComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/ThreadPool/ThreadPool.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

class WorldTextureRenderFormatFixture
{
public:
    World world;
    EventBus bus;
    StubRenderer renderer;
    Camera2D camera;
    ThreadPool threadPool{1};
    StubTextureFactory textureFactory;
    StubTextureLoader textureLoader{textureFactory};
    ResourceManager resourceManager{threadPool};
    WorldTextureRenderFormat format;
    RenderContext context;

    WorldTextureRenderFormatFixture() :
        format(WorldTextureRenderFormat::Config{
            .renderer           = this->renderer,
            .camera             = this->camera,
            .resourceManager    = this->resourceManager,
            .textureLoader      = this->textureLoader
        }),
        context{ this->world, this->bus }
    {
        auto& comp = this->world.components();
        comp.registerComponent<SpriteComponent>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<OrientationComponent>();
        comp.registerComponent<AnimationControllerComponent>();
        comp.registerComponent<ParallaxComponent>();
        comp.registerComponent<VisualEffectsComponent>();

        this->camera.setPosition(50.f, 20.f);
        this->camera.setZoom(2.f);
        this->format.setViewport(Viewport{ 0, 0, 800, 600});
    }
};

TEST_CASE_METHOD(WorldTextureRenderFormatFixture, "WorldTextureRenderFormat submits base and visual effect commands",
    "[unit][world_texture_render_format]"
) {
    const Entity entity = this->world.entities().create();
    auto& comp = this->world.components();
    
    comp.add<TransformComponent>(entity, TransformComponent{Position{100.f, 80.f}, Position{-1.5f, 2.f}, 30.f});
    comp.add<SpriteComponent>(entity, SpriteComponent{
        .texturePath    = "assets/sprites/fighter.png",
        .size           = Dimension2D {20.f, 10.f},
        .source         = Rectangle {Position{2.f, 3.f}, Dimension2D{4.f, 5.f}},
        .useSourceRect  = true
    });
    comp.add<RenderComponent>(entity, RenderComponent {7, 9});
    comp.add<ParallaxComponent>(entity, ParallaxComponent{Position{0.5f, 1.f}});
    comp.add<OrientationComponent>(entity, OrientationComponent { Orientation::Left });

    AnimationControllerComponent animationController;
    animationController.animations.symmetric = true;
    comp.add<AnimationControllerComponent>(entity, animationController);

    VisualEffectsComponent fx;
    fx.textureEffects.push_back([](DrawTextureBatch& batch, DrawTextureCommand& cmd) 
    {
        DrawTextureCommand echo = cmd;
        echo.tint = Color {1, 2, 3, 255};
        echo.dest.position.x += 3.f;
        batch.add(echo);
    });
    comp.add<VisualEffectsComponent>(entity, fx);

    this->format.render(this->context);

    REQUIRE(this->renderer.textureCalls.size() == 2);

    const auto& effectCmd = this->renderer.textureCalls[0];
    REQUIRE(effectCmd.tint == Color {1, 2, 3, 255});
    REQUIRE(effectCmd.dest.position.x == Catch::Approx(538.f));

    const auto& baseCmd = this->renderer.textureCalls[1];
    REQUIRE(baseCmd.texture != nullptr);
    REQUIRE(baseCmd.dest.position.x == Catch::Approx(535.f));
    REQUIRE(baseCmd.dest.position.y == Catch::Approx(410.f));
    REQUIRE(baseCmd.dest.size.width == Catch::Approx(30.f));
    REQUIRE(baseCmd.dest.size.height == Catch::Approx(20.f));
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
    auto& comp = this->world.components();

    comp.add<TransformComponent>(entity, TransformComponent{Position{10.f, 20.f}, Position{1.f, 1.f}, 0.f});
    comp.add<SpriteComponent>(entity, SpriteComponent{
        .texturePath    = "",
        .size           = Dimension2D {20.f, 10.f},
        .source         = Rectangle{},
        .useSourceRect  = false
    });
    comp.add<RenderComponent>(entity, RenderComponent { 0, 0 });

    this->format.render(this->context);

    REQUIRE(this->renderer.textureCalls.empty());
}

TEST_CASE_METHOD(WorldTextureRenderFormatFixture, "WorldTextureRenderFormat respects non-symmetric animation orientation",
    "[unit][world_texture_render_format]"
) {
    const Entity entity = this->world.entities().create();
    auto& comp = this->world.components();
    
    comp.add<TransformComponent>(entity, TransformComponent{Position{100.f, 80.f}, Position{-1.f, 1.f}, 0.f});
    comp.add<SpriteComponent>(entity, SpriteComponent{
        .texturePath    = "assets/sprites/fighter.png",
        .size           = Dimension2D {20.f, 10.f},
        .source         = Rectangle{},
        .useSourceRect  = false
    });
    comp.add<RenderComponent>(entity, RenderComponent { 0, 0 });
    comp.add<OrientationComponent>(entity, OrientationComponent { Orientation::Left });

    AnimationControllerComponent animationController;
    animationController.animations.symmetric = false;
    comp.add<AnimationControllerComponent>(entity, animationController);

    this->format.render(this->context);

    REQUIRE(this->renderer.textureCalls.size() == 1);
    REQUIRE(this->renderer.textureCalls[0].flipX == false);
}
