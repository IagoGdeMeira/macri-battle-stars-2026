#include "game/include/LocalToWorldSystem/LocalToWorldSystem.h"

#include "domain/components/ChildrenComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <catch2/catch_test_macros.hpp>

class LocalToWorldSystemFixture
{
public:
    LocalToWorldSystemFixture() : context{ this->world, this->bus, this->commandBuffer, 0.016f }
    {
        auto& comp = this->world.components();
        comp.registerComponent<ChildrenComponent>();
        comp.registerComponent<LocalTransform>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<TransformComponent>();
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
    auto& comp = this->world.components();
    auto& entities = this->world.entities();

    const auto parent = entities.create();
    comp.add<TransformComponent>(parent, TransformComponent{10.f, 20.f, 2.f, 0.5f, 0.f});

    const auto child = entities.create();
    comp.add<TransformComponent>(child, TransformComponent{0.f, 0.f, 1.f, 1.f, 0.f});
    comp.add<LocalTransform>(child, LocalTransform{3.5f, -2.f, 1.5f, 2.f, 0.25f});
    comp.add<ParentComponent>(child, ParentComponent{parent});
    comp.add<ChildrenComponent>(parent, ChildrenComponent{ {child} });

    this->system.update(this->context);

    const auto& childTransform = comp.get<TransformComponent>(child);
    REQUIRE(childTransform.position.x == 17.f);
    REQUIRE(childTransform.position.y == 19.f);
    REQUIRE(childTransform.rotation == 0.25f);
    REQUIRE(childTransform.scale.x == 3.f);
    REQUIRE(childTransform.scale.y == 1.f);
}

TEST_CASE_METHOD(LocalToWorldSystemFixture, "LocalToWorldSystem keeps child unchanged when parent has no TransformComponent",
    "[unit][local_to_world_system]"
) {
    auto& comp = this->world.components();
    auto& entities = this->world.entities();

    const auto parent = entities.create();
    const auto child = entities.create();

    comp.add<TransformComponent>(child, TransformComponent{7.f, 9.f, 1.2f, 0.8f, 0.5f});
    comp.add<LocalTransform>(child, LocalTransform{4.f, 5.f, 2.f, 2.f, 1.f});
    comp.add<ParentComponent>(child, ParentComponent{ parent });

    this->system.update(this->context);

    const auto& childTransform = comp.get<TransformComponent>(child);
    REQUIRE(childTransform.position.x == 7.f);
    REQUIRE(childTransform.position.y == 9.f);
    REQUIRE(childTransform.rotation == 0.5f);
    REQUIRE(childTransform.scale.x == 1.2f);
    REQUIRE(childTransform.scale.y == 0.8f);
}

TEST_CASE_METHOD(LocalToWorldSystemFixture, "LocalToWorldSystem updates each child with its respective parent",
    "[unit][local_to_world_system]"
) {
    auto& comp = this->world.components();
    auto& entities = this->world.entities();

    const auto parentA = entities.create();
    comp.add<TransformComponent>(parentA, TransformComponent{100.f, 50.f, 1.f, 1.f, 0.f});

    const auto parentB = entities.create();
    comp.add<TransformComponent>(parentB, TransformComponent{-20.f, 30.f, 1.f, 1.f, 0.f});

    const auto childA = entities.create();
    comp.add<TransformComponent>(childA, TransformComponent{0.f, 0.f, 1.f, 1.f, 0.f});
    comp.add<LocalTransform>(childA, LocalTransform{10.f, 15.f, 1.f, 1.f, 0.f});
    comp.add<ParentComponent>(childA, ParentComponent{ parentA });

    const auto childB = entities.create();
    comp.add<TransformComponent>(childB, TransformComponent{0.f, 0.f, 1.f, 1.f, 0.f});
    comp.add<LocalTransform>(childB, LocalTransform{-5.f, -8.f, 1.f, 1.f, 0.f});
    comp.add<ParentComponent>(childB, ParentComponent{ parentB });

    comp.add<ChildrenComponent>(parentA, ChildrenComponent{ {childA} });
    comp.add<ChildrenComponent>(parentB, ChildrenComponent{ {childB} });

    this->system.update(this->context);

    const auto& childATransform = comp.get<TransformComponent>(childA);
    REQUIRE(childATransform.position.x == 110.f);
    REQUIRE(childATransform.position.y == 65.f);

    const auto& childBTransform = comp.get<TransformComponent>(childB);
    REQUIRE(childBTransform.position.x == -25.f);
    REQUIRE(childBTransform.position.y == 22.f);
}
