#include "game/include/Camera2D/Camera2D.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Camera2D updates position and movement", "[unit][camera_2d]")
{
    Camera2D camera;

    camera.setPosition(10.f, 20.f);

    REQUIRE(camera.getPosition().x == 10.f);
    REQUIRE(camera.getPosition().y == 20.f);
    REQUIRE(camera.getZoom() == 1.f);

    camera.move(-3.f, 5.f);

    REQUIRE(camera.getPosition().x == 7.f);
    REQUIRE(camera.getPosition().y == 25.f);
}

TEST_CASE("Camera2D updates zoom", "[unit][camera_2d]")
{
    Camera2D camera;
    camera.setZoom(1.75f);
    
    REQUIRE(camera.getZoom() == 1.75f);
}
