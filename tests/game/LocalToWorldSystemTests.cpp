#include "../../src/game/include/LocalToWorldSystem/LocalToWorldSystem.h"

#include "../../src/domain/components/LocalPosition.h"
#include "../../src/domain/components/ParentComponent.h"
#include "../../src/domain/components/Position.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"

#include <catch2/catch_test_macros.hpp>

class LocalToWorldSystemFixture
{
public:
    LocalToWorldSystemFixture() : context{ world, bus, commandBuffer, 0.016f }
    {
        this->world.components().registerComponent<Position>();
        this->world.components().registerComponent<LocalPosition>();
        this->world.components().registerComponent<ParentComponent>();
    }

protected:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    LocalToWorldSystem system;
    UpdateContext context;
};

TEST_CASE_METHOD(LocalToWorldSystemFixture, "LocalToWorldSystem updates world position from parent and local offset",
    "[unit][local_to_world_system]"
) {
    const auto parent = this->world.entities().create();
    this->world.components().add<Position>(parent, Position{ 10.0f, 20.0f });

    const auto child = this->world.entities().create();
    this->world.components().add<Position>(child, Position{ 0.0f, 0.0f });
    this->world.components().add<LocalPosition>(child, LocalPosition{ 3.5f, -2.0f });
    this->world.components().add<ParentComponent>(child, ParentComponent{ parent });

    this->system.update(this->context);

    const auto& childPosition = this->world.components().get<Position>(child);
    REQUIRE(childPosition.x == 13.5f);
    REQUIRE(childPosition.y == 18.0f);
}

TEST_CASE_METHOD(LocalToWorldSystemFixture, "LocalToWorldSystem keeps child unchanged when parent has no Position",
    "[unit][local_to_world_system]"
) {
    const auto parent = this->world.entities().create();

    const auto child = this->world.entities().create();
    this->world.components().add<Position>(child, Position{ 7.0f, 9.0f });
    this->world.components().add<LocalPosition>(child, LocalPosition{ 4.0f, 5.0f });
    this->world.components().add<ParentComponent>(child, ParentComponent{ parent });

    this->system.update(this->context);

    const auto& childPosition = this->world.components().get<Position>(child);
    REQUIRE(childPosition.x == 7.0f);
    REQUIRE(childPosition.y == 9.0f);
}

TEST_CASE_METHOD(LocalToWorldSystemFixture, "LocalToWorldSystem updates each child with its respective parent",
    "[unit][local_to_world_system]"
) {
    const auto parentA = this->world.entities().create();
    this->world.components().add<Position>(parentA, Position{ 100.0f, 50.0f });

    const auto parentB = this->world.entities().create();
    this->world.components().add<Position>(parentB, Position{ -20.0f, 30.0f });

    const auto childA = this->world.entities().create();
    this->world.components().add<Position>(childA, Position{ 0.0f, 0.0f });
    this->world.components().add<LocalPosition>(childA, LocalPosition{ 10.0f, 15.0f });
    this->world.components().add<ParentComponent>(childA, ParentComponent{ parentA });

    const auto childB = this->world.entities().create();
    this->world.components().add<Position>(childB, Position{ 0.0f, 0.0f });
    this->world.components().add<LocalPosition>(childB, LocalPosition{ -5.0f, -8.0f });
    this->world.components().add<ParentComponent>(childB, ParentComponent{ parentB });

    this->system.update(this->context);

    const auto& childAPosition = this->world.components().get<Position>(childA);
    REQUIRE(childAPosition.x == 110.0f);
    REQUIRE(childAPosition.y == 65.0f);

    const auto& childBPosition = this->world.components().get<Position>(childB);
    REQUIRE(childBPosition.x == -25.0f);
    REQUIRE(childBPosition.y == 22.0f);
}
