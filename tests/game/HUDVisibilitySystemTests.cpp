#include "game/include/HUDVisibilitySystem/HUDVisibilitySystem.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/ChildrenComponent.h"
#include "domain/components/HUDEntityTag.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/UITransform.h"
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
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<UITransform>();
    }

    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    HUDVisibilitySystem system;
    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.016f};

    Entity createHudRoot()
    {
        auto& comp = this->world.components();
        Entity root = this->world.entities().create();
        comp.add<ActiveComponent>(root, ActiveComponent{true});
        comp.add<HUDEntityTag>(root, HUDEntityTag{});
        comp.add<ChildrenComponent>(root, ChildrenComponent{});
        return root;
    }

    Entity createChild(Entity parent, bool active = true)
    {
        auto& comp = this->world.components();
        Entity child = this->world.entities().create();
        comp.add<ActiveComponent>(child, ActiveComponent{active});
        comp.add<ParentComponent>(child, ParentComponent{parent});
        comp.add<UITransform>(child, UITransform{});

        comp.get<ChildrenComponent>(parent).children.push_back(child);
        return child;
    }
};

TEST_CASE_METHOD(HUDVisibilitySystemFixture, "HUDVisibilitySystem hides all HUD entities recursively",
    "[unit][hud_visibility_system]")
{
    Entity root = this->createHudRoot();
    Entity child1 = this->createChild(root);
    Entity child2 = this->createChild(root);
    Entity grandchild = this->createChild(child1);

    this->system.setVisible(false);
    this->system.update(this->ctx);

    auto& comp = this->world.components();
    REQUIRE(comp.get<ActiveComponent>(root).active == false);
    REQUIRE(comp.get<ActiveComponent>(child1).active == false);
    REQUIRE(comp.get<ActiveComponent>(child2).active == false);
    REQUIRE(comp.get<ActiveComponent>(grandchild).active == false);
}

TEST_CASE_METHOD(HUDVisibilitySystemFixture, "HUDVisibilitySystem shows all HUD entities recursively",
    "[unit][hud_visibility_system]")
{
    Entity root = this->createHudRoot();
    Entity child = this->createChild(root, false);

    this->system.setVisible(true);
    this->system.update(this->ctx);

    auto& comp = this->world.components();
    REQUIRE(comp.get<ActiveComponent>(root).active == true);
    REQUIRE(comp.get<ActiveComponent>(child).active == true);
}

TEST_CASE_METHOD(HUDVisibilitySystemFixture, "HUDVisibilitySystem does nothing when visibility is unchanged",
    "[unit][hud_visibility_system]")
{
    Entity root = this->createHudRoot();
    Entity child = this->createChild(root);

    this->system.setVisible(false);
    this->system.update(this->ctx);

    auto& comp = this->world.components();
    REQUIRE(comp.get<ActiveComponent>(root).active == false);
    
    comp.get<ActiveComponent>(child).active = true;

    this->system.update(this->ctx);
    REQUIRE(comp.get<ActiveComponent>(child).active == true);
}

TEST_CASE_METHOD(HUDVisibilitySystemFixture, "HUDVisibilitySystem setVisible without update does not change state",
    "[unit][hud_visibility_system]")
{
    Entity root = this->createHudRoot();

    this->system.setVisible(false);
    auto& comp = this->world.components();
    REQUIRE(comp.get<ActiveComponent>(root).active == true);
}
