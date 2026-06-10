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
    LocalToWorldSystemFixture() : context{ this->world, this->bus, this->commandBuffer, 0.016f }
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

TEST_CASE_METHOD(LocalToWorldSystemFixture,
    "LocalToWorldSystem updates world transform from parent and local transform",
    "[unit][local_to_world_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();

    const auto parent = entities.create();
    components.add<TransformComponent>(parent, TransformComponent{10.f, 20.f, 2.f, 0.5f, 0.f});

    const auto child = entities.create();
    components.add<TransformComponent>(child, TransformComponent{0.f, 0.f, 1.f, 1.f, 0.f});
    components.add<LocalTransform>(child, LocalTransform{3.5f, -2.f, 1.5f, 2.f, 0.25f});
    components.add<ParentComponent>(child, ParentComponent{parent});

    this->system.update(this->context);

    const auto& childTransform = components.get<TransformComponent>(child);
    REQUIRE(childTransform.position.x == 17.f);
    REQUIRE(childTransform.position.y == 19.f);
    REQUIRE(childTransform.rotation == 0.25f);
    REQUIRE(childTransform.scale.x == 3.f);
    REQUIRE(childTransform.scale.y == 1.f);
}

TEST_CASE_METHOD(LocalToWorldSystemFixture,
    "LocalToWorldSystem keeps child unchanged when parent has no TransformComponent",
    "[unit][local_to_world_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();

    const auto parent = entities.create();
    const auto child = entities.create();

    components.add<TransformComponent>(child, TransformComponent{7.f, 9.f, 1.2f, 0.8f, 0.5f});
    components.add<LocalTransform>(child, LocalTransform{4.f, 5.f, 2.f, 2.f, 1.f});
    components.add<ParentComponent>(child, ParentComponent{ parent });

    this->system.update(this->context);

    const auto& childTransform = components.get<TransformComponent>(child);
    REQUIRE(childTransform.position.x == 7.f);
    REQUIRE(childTransform.position.y == 9.f);
    REQUIRE(childTransform.rotation == 0.5f);
    REQUIRE(childTransform.scale.x == 1.2f);
    REQUIRE(childTransform.scale.y == 0.8f);
}

TEST_CASE_METHOD(LocalToWorldSystemFixture,
    "LocalToWorldSystem updates each child with its respective parent",
    "[unit][local_to_world_system]"
) {
    auto& components = this->world.components();
    auto& entities = this->world.entities();

    const auto parentA = entities.create();
    components.add<TransformComponent>(parentA, TransformComponent{100.f, 50.f, 1.f, 1.f, 0.f});

    const auto parentB = entities.create();
    components.add<TransformComponent>(parentB, TransformComponent{-20.f, 30.f, 1.f, 1.f, 0.f});

    const auto childA = entities.create();
    components.add<TransformComponent>(childA, TransformComponent{0.f, 0.f, 1.f, 1.f, 0.f});
    components.add<LocalTransform>(childA, LocalTransform{10.f, 15.f, 1.f, 1.f, 0.f});
    components.add<ParentComponent>(childA, ParentComponent{ parentA });

    const auto childB = entities.create();
    components.add<TransformComponent>(childB, TransformComponent{0.f, 0.f, 1.f, 1.f, 0.f});
    components.add<LocalTransform>(childB, LocalTransform{-5.f, -8.f, 1.f, 1.f, 0.f});
    components.add<ParentComponent>(childB, ParentComponent{ parentB });

    this->system.update(this->context);

    const auto& childATransform = components.get<TransformComponent>(childA);
    REQUIRE(childATransform.position.x == 110.f);
    REQUIRE(childATransform.position.y == 65.f);

    const auto& childBTransform = components.get<TransformComponent>(childB);
    REQUIRE(childBTransform.position.x == -25.f);
    REQUIRE(childBTransform.position.y == 22.f);
}
