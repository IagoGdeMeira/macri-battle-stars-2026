#include "game/utils/WorldRenderUtils/WorldRenderUtils.h"

#include "domain/components/ParallaxComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/include/World/World.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("WorldRenderUtils::worldToScreen applies camera position", "[unit][world_render_utils]")
{
    Camera2D camera;
    camera.setPosition(10.f, 20.f);
    Position worldPos {100.f, 60.f};
    const Position screen = WorldRenderUtils::worldToScreen(camera, worldPos);

    REQUIRE(screen.x == Catch::Approx(490.f));
    REQUIRE(screen.y == Catch::Approx(340.f));
}

TEST_CASE("WorldRenderUtils::worldToScreen applies parallax factors", "[unit][world_render_utils]")
{
    Camera2D camera;
    camera.setPosition(10.f, 20.f);
    Position worldPos {100.f, 60.f};
    const Position screen = WorldRenderUtils::worldToScreen(camera, worldPos, Position{0.5f, 2.f});

    REQUIRE(screen.x == Catch::Approx(495.f));
    REQUIRE(screen.y == Catch::Approx(320.f));
}

TEST_CASE("WorldRenderUtils::resolveParallax reads component when present", "[unit][world_render_utils]")
{
    World world;
    auto& comp = world.components();

    comp.registerComponent<ParallaxComponent>();

    Entity entity = world.entities().create();
    comp.add<ParallaxComponent>(entity, ParallaxComponent{Position{0.3f, 0.7f}});

    const Position parallax = WorldRenderUtils::resolveParallax(world, entity);

    REQUIRE(parallax.x == Catch::Approx(0.3f));
    REQUIRE(parallax.y == Catch::Approx(0.7f));
}

TEST_CASE("WorldRenderUtils::resolveParallax returns default when absent", "[unit][world_render_utils]")
{
    World world;
    auto& comp = world.components();
    
    comp.registerComponent<ParallaxComponent>();
    comp.registerComponent<ParentComponent>();

    Entity entity = world.entities().create();

    const Position parallax = WorldRenderUtils::resolveParallax(world, entity);

    REQUIRE(parallax.x == Catch::Approx(1.f));
    REQUIRE(parallax.y == Catch::Approx(1.f));
}

TEST_CASE("WorldRenderUtils::computeSpriteTransform applies scaled dimensions and flips", "[unit][world_render_utils]"
) {
    Camera2D camera;
    camera.setZoom(1.5f);

    Rectangle spriteConfig;
    spriteConfig.position = Position { -2.f, 3.f };
    spriteConfig.size = Dimension2D { 16.f, 8.f };

    DrawTextureCommand cmd;
    WorldRenderUtils::computeSpriteTransform(spriteConfig, cmd);

    REQUIRE(cmd.dest.size.width == Catch::Approx(32.f));
    REQUIRE(cmd.dest.size.height == Catch::Approx(24.f));
    REQUIRE(cmd.flipX == true);
    REQUIRE(cmd.flipY == false);
}

TEST_CASE("WorldRenderUtils::computeSpriteTransform centers sprite on dest.position", "[unit][world_render_utils]")
{
    Camera2D camera;
    Rectangle spriteConfig;
    spriteConfig.position = Position {2.f, 3.f};
    spriteConfig.size = Dimension2D {16.f, 8.f};

    DrawTextureCommand cmd;
    cmd.dest.position = {100.f, 200.f};

    WorldRenderUtils::computeSpriteTransform(spriteConfig, cmd);

    REQUIRE(cmd.dest.size.width == Catch::Approx(32.f));
    REQUIRE(cmd.dest.size.height == Catch::Approx(24.f));

    REQUIRE(cmd.dest.position.x == Catch::Approx(84.f));
    REQUIRE(cmd.dest.position.y == Catch::Approx(188.f));
    
    REQUIRE(cmd.flipX == false);
    REQUIRE(cmd.flipY == false);
}

TEST_CASE("WorldRenderUtils::computeSpriteTransform handles zero scale", "[unit][world_render_utils]")
{
    Camera2D camera;
    Rectangle spriteConfig;
    spriteConfig.position = Position {0.f, 0.f};
    spriteConfig.size = Dimension2D {16.f, 8.f};

    DrawTextureCommand cmd;
    cmd.dest.position = {10.f, 20.f};
    WorldRenderUtils::computeSpriteTransform(spriteConfig, cmd);

    REQUIRE(cmd.dest.size.width == Catch::Approx(0.f));
    REQUIRE(cmd.dest.size.height == Catch::Approx(0.f));
    REQUIRE(cmd.dest.position.x == Catch::Approx(10.f));
    REQUIRE(cmd.dest.position.y == Catch::Approx(20.f));
    REQUIRE(cmd.flipX == false);
    REQUIRE(cmd.flipY == false);
}
