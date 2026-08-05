#include "game/utils/CollisionUtils/CollisionUtils.h"

#include "domain/components/LocalTransform.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/World/World.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>

class CollisionUtilsFixture
{
public:
    CollisionUtilsFixture()
    {
        auto& comp = this->world.components();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<LocalTransform>();
    }

protected:
    World world;
};

TEST_CASE_METHOD(CollisionUtilsFixture,
    "CollisionUtils::updateWorldTransform applies simple translation without rotation or scale",
    "[unit][collision_utils]")
{
    auto& comp = this->world.components();
    auto& entities = this->world.entities();
    const auto parent = entities.create();
    const auto child  = entities.create();

    comp.add<TransformComponent>(parent, TransformComponent{});
    comp.add<TransformComponent>(child, TransformComponent{});
    comp.add<LocalTransform>(child, LocalTransform{});

    auto& parentT = comp.get<TransformComponent>(parent);
    parentT.position = { 100.f, 50.f };
    parentT.scale = { 1.f, 1.f };
    parentT.rotation = 0.f;

    auto& local = comp.get<LocalTransform>(child);
    local.position = { 30.f, -20.f };
    local.scale = { 1.f, 1.f };
    local.rotation = 0.f;

    CollisionUtils::updateWorldTransform(this->world, child, parent);

    const auto& childT = comp.get<TransformComponent>(child);
    REQUIRE(childT.position.x == Catch::Approx(130.f));
    REQUIRE(childT.position.y == Catch::Approx(30.f));
    REQUIRE(childT.rotation == Catch::Approx(0.f));
    REQUIRE(childT.scale.x == Catch::Approx(1.f));
    REQUIRE(childT.scale.y == Catch::Approx(1.f));
}

TEST_CASE_METHOD(CollisionUtilsFixture, "CollisionUtils::updateWorldTransform applies parent scale to local offset",
    "[unit][collision_utils]")
{
    auto& comp = this->world.components();
    auto& entities = this->world.entities();
    const auto parent = entities.create();
    const auto child  = entities.create();

    comp.add<TransformComponent>(parent, TransformComponent{});
    comp.add<TransformComponent>(child, TransformComponent{});
    comp.add<LocalTransform>(child, LocalTransform{});

    auto& parentT = comp.get<TransformComponent>(parent);
    parentT.position = { 10.f, 20.f };
    parentT.scale = { 2.f, 3.f };
    parentT.rotation = 0.f;

    auto& local = comp.get<LocalTransform>(child);
    local.position = { 5.f, -4.f };
    local.scale = { 1.f, 1.f };
    local.rotation = 0.f;

    CollisionUtils::updateWorldTransform(this->world, child, parent);

    const auto& childT = comp.get<TransformComponent>(child);
    REQUIRE(childT.position.x == Catch::Approx(20.f));
    REQUIRE(childT.position.y == Catch::Approx(8.f));
    REQUIRE(childT.scale.x == Catch::Approx(2.f));
    REQUIRE(childT.scale.y == Catch::Approx(3.f));
}

TEST_CASE_METHOD(CollisionUtilsFixture, "CollisionUtils::updateWorldTransform rotates local offset by parent rotation",
    "[unit][collision_utils]")
{
    auto& comp = this->world.components();
    auto& entities = this->world.entities();
    const auto parent = entities.create();
    const auto child  = entities.create();

    comp.add<TransformComponent>(parent, TransformComponent{});
    comp.add<TransformComponent>(child, TransformComponent{});
    comp.add<LocalTransform>(child, LocalTransform{});

    auto& parentT = comp.get<TransformComponent>(parent);
    parentT.position = { 0.f, 0.f };
    parentT.scale = { 1.f, 1.f };
    parentT.rotation = 1.57079632679f;

    auto& local = comp.get<LocalTransform>(child);
    local.position = { 10.f, 0.f };
    local.scale = { 1.f, 1.f };
    local.rotation = 0.f;

    CollisionUtils::updateWorldTransform(this->world, child, parent);

    const auto& childT = comp.get<TransformComponent>(child);
    REQUIRE(childT.position.x == Catch::Approx(0.f).margin(1e-4));
    REQUIRE(childT.position.y == Catch::Approx(10.f).margin(1e-4));
    REQUIRE(childT.rotation == Catch::Approx(1.57079632679f));
}

TEST_CASE_METHOD(CollisionUtilsFixture, "CollisionUtils::updateWorldTransform combines parent rotation and scale",
    "[unit][collision_utils]")
{
    auto& comp = this->world.components();
    auto& entities = this->world.entities();
    const auto parent = entities.create();
    const auto child  = entities.create();

    comp.add<TransformComponent>(parent, TransformComponent{});
    comp.add<TransformComponent>(child, TransformComponent{});
    comp.add<LocalTransform>(child, LocalTransform{});

    auto& parentT = comp.get<TransformComponent>(parent);
    parentT.position = { 50.f, 0.f };
    parentT.scale = { 2.f, 2.f };
    parentT.rotation = 0.785398163397f;

    auto& local = comp.get<LocalTransform>(child);
    local.position = { 10.f, 0.f };
    local.scale = { 1.f, 1.f };
    local.rotation = 0.f;

    CollisionUtils::updateWorldTransform(this->world, child, parent);

    const auto& childT = comp.get<TransformComponent>(child);
    const float cos45 = std::cos(0.785398163397f);
    const float sin45 = std::sin(0.785398163397f);
    const float expectedX = 50.f + (10.f * cos45) * 2.f;
    const float expectedY = 0.f  + (10.f * sin45) * 2.f;

    REQUIRE(childT.position.x == Catch::Approx(expectedX).margin(1e-4));
    REQUIRE(childT.position.y == Catch::Approx(expectedY).margin(1e-4));
    REQUIRE(childT.rotation == Catch::Approx(0.785398163397f));
    REQUIRE(childT.scale.x == Catch::Approx(2.f));
    REQUIRE(childT.scale.y == Catch::Approx(2.f));
}

TEST_CASE_METHOD(CollisionUtilsFixture, "CollisionUtils::updateWorldTransform inherits local rotation and scale from child",
    "[unit][collision_utils]")
{
    auto& comp = this->world.components();
    auto& entities = this->world.entities();
    const auto parent = entities.create();
    const auto child  = entities.create();

    comp.add<TransformComponent>(parent, TransformComponent{});
    comp.add<TransformComponent>(child, TransformComponent{});
    comp.add<LocalTransform>(child, LocalTransform{});

    auto& parentT = comp.get<TransformComponent>(parent);
    parentT.position = { 0.f, 0.f };
    parentT.scale = { 2.f, 2.f };
    parentT.rotation = 0.5f;

    auto& local = comp.get<LocalTransform>(child);
    local.position = { 0.f, 0.f };
    local.scale = { 0.5f, 1.5f };
    local.rotation = 0.3f;

    CollisionUtils::updateWorldTransform(this->world, child, parent);

    const auto& childT = comp.get<TransformComponent>(child);
    REQUIRE(childT.position.x == Catch::Approx(0.f));
    REQUIRE(childT.position.y == Catch::Approx(0.f));
    REQUIRE(childT.rotation == Catch::Approx(0.8f));
    REQUIRE(childT.scale.x == Catch::Approx(1.0f));
    REQUIRE(childT.scale.y == Catch::Approx(3.0f));
}

TEST_CASE_METHOD(CollisionUtilsFixture,
    "CollisionUtils::updateWorldTransform returns early when child lacks LocalTransform",
    "[unit][collision_utils]")
{
    auto& comp = this->world.components();
    auto& entities = this->world.entities();
    const auto parent = entities.create();
    const auto child  = entities.create();

    comp.add<TransformComponent>(parent, TransformComponent{});
    comp.add<TransformComponent>(child, TransformComponent{});

    auto& childT = comp.get<TransformComponent>(child);
    childT.position = { 99.f, 88.f };

    CollisionUtils::updateWorldTransform(this->world, child, parent);

    REQUIRE(childT.position.x == Catch::Approx(99.f));
    REQUIRE(childT.position.y == Catch::Approx(88.f));
}

TEST_CASE_METHOD(CollisionUtilsFixture,
    "CollisionUtils::updateWorldTransform returns early when parent lacks TransformComponent",
    "[unit][collision_utils]")
{
    auto& comp = this->world.components();
    auto& entities = this->world.entities();
    const auto parent = entities.create();
    const auto child  = entities.create();

    comp.add<TransformComponent>(child, TransformComponent{});
    comp.add<LocalTransform>(child, LocalTransform{});

    auto& childT = comp.get<TransformComponent>(child);
    childT.position = { 55.f, 44.f };

    CollisionUtils::updateWorldTransform(this->world, child, parent);

    REQUIRE(childT.position.x == Catch::Approx(55.f));
    REQUIRE(childT.position.y == Catch::Approx(44.f));
}
