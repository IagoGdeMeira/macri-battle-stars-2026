#include "../../src/game/include/LocalToWorldSystem/LocalToWorldSystem.h"

#include "../../src/domain/components/LocalTransform.h"
#include "../../src/domain/components/ParentComponent.h"
#include "../../src/domain/components/TransformComponent.h"
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
        this->world.components().registerComponent<TransformComponent>();
        this->world.components().registerComponent<LocalTransform>();
        this->world.components().registerComponent<ParentComponent>();
    }

protected:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    LocalToWorldSystem system;
    UpdateContext context;
};

TEST_CASE_METHOD(LocalToWorldSystemFixture, "LocalToWorldSystem updates world transform from parent and local transform",
    "[unit][local_to_world_system]"
) {
    const auto parent = this->world.entities().create();
    this->world.components().add<TransformComponent>(parent, TransformComponent{ 10.0f, 20.0f, 2.0f, 0.5f, 0.0f });

    const auto child = this->world.entities().create();
    this->world.components().add<TransformComponent>(child, TransformComponent{ 0.0f, 0.0f, 1.0f, 1.0f, 0.0f });
    this->world.components().add<LocalTransform>(child, LocalTransform{ 3.5f, -2.0f, 1.5f, 2.0f, 0.25f });
    this->world.components().add<ParentComponent>(child, ParentComponent{ parent });

    this->system.update(this->context);

    const auto& childTransform = this->world.components().get<TransformComponent>(child);
    REQUIRE(childTransform.x == 17.0f);
    REQUIRE(childTransform.y == 19.0f);
    REQUIRE(childTransform.rotation == 0.25f);
    REQUIRE(childTransform.scaleX == 3.0f);
    REQUIRE(childTransform.scaleY == 1.0f);
}

TEST_CASE_METHOD(LocalToWorldSystemFixture, "LocalToWorldSystem keeps child unchanged when parent has no TransformComponent",
    "[unit][local_to_world_system]"
) {
    const auto parent = this->world.entities().create();

    const auto child = this->world.entities().create();
    this->world.components().add<TransformComponent>(child, TransformComponent{ 7.0f, 9.0f, 1.2f, 0.8f, 0.5f });
    this->world.components().add<LocalTransform>(child, LocalTransform{ 4.0f, 5.0f, 2.0f, 2.0f, 1.0f });
    this->world.components().add<ParentComponent>(child, ParentComponent{ parent });

    this->system.update(this->context);

    const auto& childTransform = this->world.components().get<TransformComponent>(child);
    REQUIRE(childTransform.x == 7.0f);
    REQUIRE(childTransform.y == 9.0f);
    REQUIRE(childTransform.rotation == 0.5f);
    REQUIRE(childTransform.scaleX == 1.2f);
    REQUIRE(childTransform.scaleY == 0.8f);
}

TEST_CASE_METHOD(LocalToWorldSystemFixture, "LocalToWorldSystem updates each child with its respective parent",
    "[unit][local_to_world_system]"
) {
    const auto parentA = this->world.entities().create();
    this->world.components().add<TransformComponent>(parentA, TransformComponent{ 100.0f, 50.0f, 1.0f, 1.0f, 0.0f });

    const auto parentB = this->world.entities().create();
    this->world.components().add<TransformComponent>(parentB, TransformComponent{ -20.0f, 30.0f, 1.0f, 1.0f, 0.0f });

    const auto childA = this->world.entities().create();
    this->world.components().add<TransformComponent>(childA, TransformComponent{ 0.0f, 0.0f, 1.0f, 1.0f, 0.0f });
    this->world.components().add<LocalTransform>(childA, LocalTransform{ 10.0f, 15.0f, 1.0f, 1.0f, 0.0f });
    this->world.components().add<ParentComponent>(childA, ParentComponent{ parentA });

    const auto childB = this->world.entities().create();
    this->world.components().add<TransformComponent>(childB, TransformComponent{ 0.0f, 0.0f, 1.0f, 1.0f, 0.0f });
    this->world.components().add<LocalTransform>(childB, LocalTransform{ -5.0f, -8.0f, 1.0f, 1.0f, 0.0f });
    this->world.components().add<ParentComponent>(childB, ParentComponent{ parentB });

    this->system.update(this->context);

    const auto& childATransform = this->world.components().get<TransformComponent>(childA);
    REQUIRE(childATransform.x == 110.0f);
    REQUIRE(childATransform.y == 65.0f);

    const auto& childBTransform = this->world.components().get<TransformComponent>(childB);
    REQUIRE(childBTransform.x == -25.0f);
    REQUIRE(childBTransform.y == 22.0f);
}
