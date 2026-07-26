#include "game/include/PushboxControllerSystem/PushboxControllerSystem.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/PushboxControllerComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <catch2/catch_test_macros.hpp>

class PushboxControllerSystemFixture
{
public:
    PushboxControllerSystemFixture()
    {
        auto& comp = this->world.components();
        comp.registerComponent<PushboxControllerComponent>();
        comp.registerComponent<ActiveComponent>();
    }

    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    PushboxControllerSystem system;
};

TEST_CASE_METHOD(PushboxControllerSystemFixture, "PushboxControllerSystem advances frames and toggles active state",
    "[unit][pushbox_controller_system]"
) {
    auto& comp = this->world.components();
    auto& entities = this->world.entities();

    Entity entity = entities.create();

    PushboxControllerComponent controller;
    controller.loop = false;

    PushboxControllerComponent::Frame frame0;
    frame0.duration = 0.1f;
    Entity pb0 = entities.create();
    comp.add<ActiveComponent>(pb0, ActiveComponent{false});
    frame0.pushboxes.push_back(pb0);

    PushboxControllerComponent::Frame frame1;
    frame1.duration = 0.2f;
    Entity pb1 = entities.create();
    comp.add<ActiveComponent>(pb1, ActiveComponent{false});
    frame1.pushboxes.push_back(pb1);

    controller.frames.push_back(frame0);
    controller.frames.push_back(frame1);
    controller.currentFrame = 0;
    controller.elapsedTime = 0.f;

    comp.add<PushboxControllerComponent>(entity, std::move(controller));

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.05f};
    this->system.update(ctx);

    REQUIRE(comp.get<ActiveComponent>(pb0).active == false);
    REQUIRE(comp.get<ActiveComponent>(pb1).active == false);

    ctx.deltaTime = 0.06f;
    this->system.update(ctx);

    REQUIRE(comp.get<ActiveComponent>(pb0).active == false);
    REQUIRE(comp.get<ActiveComponent>(pb1).active == true);
}

TEST_CASE_METHOD(PushboxControllerSystemFixture, "PushboxControllerSystem loops frames when loop is true",
    "[unit][pushbox_controller_system]"
) {
    auto& comp = this->world.components();
    auto& entities = this->world.entities();

    Entity entity = entities.create();

    PushboxControllerComponent controller;
    controller.loop = true;

    PushboxControllerComponent::Frame frame0;
    frame0.duration = 0.1f;
    Entity pb0 = entities.create();
    comp.add<ActiveComponent>(pb0, ActiveComponent{false});
    frame0.pushboxes.push_back(pb0);

    PushboxControllerComponent::Frame frame1;
    frame1.duration = 0.1f;
    Entity pb1 = entities.create();
    comp.add<ActiveComponent>(pb1, ActiveComponent{false});
    frame1.pushboxes.push_back(pb1);

    controller.frames.push_back(frame0);
    controller.frames.push_back(frame1);
    controller.currentFrame = 0;
    controller.elapsedTime = 0.f;

    comp.add<PushboxControllerComponent>(entity, std::move(controller));

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.15f};
    this->system.update(ctx);

    REQUIRE(comp.get<ActiveComponent>(pb0).active == false);
    REQUIRE(comp.get<ActiveComponent>(pb1).active == true);

    ctx.deltaTime = 0.15f;
    this->system.update(ctx);

    REQUIRE(comp.get<ActiveComponent>(pb0).active == true);
    REQUIRE(comp.get<ActiveComponent>(pb1).active == false);
}

TEST_CASE_METHOD(PushboxControllerSystemFixture, "PushboxControllerSystem stays on last frame when not looping",
    "[unit][pushbox_controller_system]"
) {
    auto& comp = this->world.components();
    auto& entities = this->world.entities();

    Entity entity = entities.create();

    PushboxControllerComponent controller;
    controller.loop = false;

    PushboxControllerComponent::Frame frame0;
    frame0.duration = 0.1f;
    Entity pb0 = entities.create();
    comp.add<ActiveComponent>(pb0, ActiveComponent{false});
    frame0.pushboxes.push_back(pb0);

    PushboxControllerComponent::Frame frame1;
    frame1.duration = 0.1f;
    Entity pb1 = entities.create();
    comp.add<ActiveComponent>(pb1, ActiveComponent{false});
    frame1.pushboxes.push_back(pb1);

    controller.frames.push_back(frame0);
    controller.frames.push_back(frame1);
    controller.currentFrame = 0;
    controller.elapsedTime = 0.f;

    comp.add<PushboxControllerComponent>(entity, std::move(controller));

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.25f};
    this->system.update(ctx);

    REQUIRE(comp.get<ActiveComponent>(pb0).active == false);
    REQUIRE(comp.get<ActiveComponent>(pb1).active == true);

    ctx.deltaTime = 0.1f;
    this->system.update(ctx);

    REQUIRE(comp.get<ActiveComponent>(pb0).active == false);
    REQUIRE(comp.get<ActiveComponent>(pb1).active == true);
}
