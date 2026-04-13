#include "../../src/game/include/Camera2D/Camera2D.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Camera2D updates position and movement",
    "[unit][camera_2d]"
) {
    Camera2D camera;

    camera.setPosition(10.0f, 20.0f);

    REQUIRE(camera.getX() == 10.0f);
    REQUIRE(camera.getY() == 20.0f);
    REQUIRE(camera.getZoom() == 1.0f);

    camera.move(-3.0f, 5.0f);

    REQUIRE(camera.getX() == 7.0f);
    REQUIRE(camera.getY() == 25.0f);
}

TEST_CASE("Camera2D updates zoom",
    "[unit][camera_2d]"
) {
    Camera2D camera;

    camera.setZoom(1.75f);

    REQUIRE(camera.getZoom() == 1.75f);
}
