#include "game/include/HUDVisibilitySystem/HUDVisibilitySystem.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/ChildrenComponent.h"
#include "domain/components/HUDEntityTag.h"
#include "domain/components/ParentComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <catch2/catch_test_macros.hpp>

class HUDVisibilitySystemFixture
{
public:
    HUDVisibilitySystemFixture()
    {
        auto& comp = this->world.components();
        comp.registerComponent<ActiveComponent>();
        comp.registerComponent<ChildrenComponent>();
        comp.registerComponent<HUDEntityTag>();
        comp.registerComponent<ParentComponent>();
    }

    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    HUDVisibilitySystem system;
    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.016f};

    Entity createRoot(bool active = true)
    {
        auto& comp = this->world.components();
        Entity e = this->world.entities().create();
        comp.add<ActiveComponent>(e, ActiveComponent{active});
        comp.add<HUDEntityTag>(e, HUDEntityTag{});
        comp.add<ChildrenComponent>(e, ChildrenComponent{});
        return e;
    }

    Entity addChild(Entity parent, bool active = true)
    {
        auto& comp = this->world.components();
        Entity child = this->world.entities().create();
        comp.add<ActiveComponent>(child, ActiveComponent{active});
        comp.add<ParentComponent>(child, ParentComponent{parent});
        if (!comp.has<ChildrenComponent>(parent)) comp.add<ChildrenComponent>(parent, ChildrenComponent{});
        if (!comp.has<ChildrenComponent>(child)) comp.add<ChildrenComponent>(child, ChildrenComponent{});
        comp.get<ChildrenComponent>(parent).children.push_back(child);
        return child;
    }
};

TEST_CASE_METHOD(HUDVisibilitySystemFixture, "Hide all HUD entities recursively",
    "[unit][hud_visibility_system]")
{
    Entity root = this->createRoot();
    Entity child1 = this->addChild(root);
    Entity child2 = this->addChild(root);
    Entity grandchild = this->addChild(child1);

    this->system.setVisible(false);
    this->system.update(this->ctx);

    auto& comp = this->world.components();
    REQUIRE(comp.get<ActiveComponent>(root).active == false);
    REQUIRE(comp.get<ActiveComponent>(child1).active == false);
    REQUIRE(comp.get<ActiveComponent>(child2).active == false);
    REQUIRE(comp.get<ActiveComponent>(grandchild).active == false);
}

TEST_CASE_METHOD(HUDVisibilitySystemFixture, "Show all HUD entities recursively",
    "[unit][hud_visibility_system]")
{
    Entity root = this->createRoot(false);
    Entity child = this->addChild(root, false);

    this->system.setVisible(false);
    this->system.update(this->ctx);
    this->system.setVisible(true);
    this->system.update(this->ctx);

    auto& comp = this->world.components();
    REQUIRE(comp.get<ActiveComponent>(root).active == true);
    REQUIRE(comp.get<ActiveComponent>(child).active == true);
}

TEST_CASE_METHOD(HUDVisibilitySystemFixture, "No change when visibility is unchanged",
    "[unit][hud_visibility_system]")
{
    Entity root = this->createRoot();
    Entity child = this->addChild(root);

    this->system.setVisible(false);
    this->system.update(this->ctx);
    auto& comp = this->world.components();
    REQUIRE(comp.get<ActiveComponent>(root).active == false);

    comp.get<ActiveComponent>(child).active = true;

    this->system.update(this->ctx);
    REQUIRE(comp.get<ActiveComponent>(child).active == true);
}

TEST_CASE_METHOD(HUDVisibilitySystemFixture, "setVisible without update does not change state",
    "[unit][hud_visibility_system]")
{
    Entity root = this->createRoot();
    this->system.setVisible(false);
    REQUIRE(this->world.components().get<ActiveComponent>(root).active == true);
}
