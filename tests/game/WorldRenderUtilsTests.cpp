#include "../../src/game/include/WorldRenderUtils/WorldRenderUtils.h"

#include "../../src/domain/components/ParallaxComponent.h"
#include "../../src/domain/include/World/World.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("WorldRenderUtils::worldToScreen applies camera, zoom and viewport", "[unit][world_render_utils]")
{
    Camera2D camera;
    camera.setPosition(10.f, 20.f);
    camera.setZoom(2.f);

    Viewport vp { 0, 0, 800, 600 };
    Position worldPos { 100.f, 60.f };

    const Position screen = WorldRenderUtils::worldToScreen(camera, worldPos, vp);

    REQUIRE(screen.x == Catch::Approx(580.f));
    REQUIRE(screen.y == Catch::Approx(380.f));
}

TEST_CASE("WorldRenderUtils::worldToScreen applies parallax factors", "[unit][world_render_utils]")
{
    Camera2D camera;
    camera.setPosition(10.f, 20.f);
    camera.setZoom(2.f);

    Viewport vp { 0, 0, 800, 600 };
    Position worldPos { 100.f, 60.f };

    const Position screen = WorldRenderUtils::worldToScreen(camera, worldPos, vp, Position{0.5f, 2.f});

    REQUIRE(screen.x == Catch::Approx(590.f));
    REQUIRE(screen.y == Catch::Approx(340.f));
}

TEST_CASE("WorldRenderUtils::resolveParallax reads component when present", "[unit][world_render_utils]")
{
    World world;
    world.components().registerComponent<ParallaxComponent>();

    Entity entity = world.entities().create();
    world.components().add<ParallaxComponent>(entity, ParallaxComponent{Position{0.3f, 0.7f}});

    const Position parallax = WorldRenderUtils::resolveParallax(world, entity);

    REQUIRE(parallax.x == Catch::Approx(0.3f));
    REQUIRE(parallax.y == Catch::Approx(0.7f));
}

TEST_CASE("WorldRenderUtils::resolveParallax returns default when absent", "[unit][world_render_utils]")
{
    World world;
    world.components().registerComponent<ParallaxComponent>();

    Entity entity = world.entities().create();

    const Position parallax = WorldRenderUtils::resolveParallax(world, entity);

    REQUIRE(parallax.x == Catch::Approx(1.f));
    REQUIRE(parallax.y == Catch::Approx(1.f));
}

TEST_CASE("WorldRenderUtils::computeSpriteTransform applies scaled dimensions and flips",
    "[unit][world_render_utils]"
) {
    Camera2D camera;
    camera.setZoom(1.5f);

    Rectangle spriteConfig;
    spriteConfig.position = Position { -2.f, 3.f };
    spriteConfig.size = Dimension2D { 16.f, 8.f };

    DrawTextureCommand cmd;
    WorldRenderUtils::computeSpriteTransform(spriteConfig, cmd);

    REQUIRE(cmd.dest.size.width == Catch::Approx(48.f));
    REQUIRE(cmd.dest.size.height == Catch::Approx(36.f));
    REQUIRE(cmd.flipX == true);
    REQUIRE(cmd.flipY == false);
}
