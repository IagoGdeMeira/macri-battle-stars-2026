#include "game/include/HurtboxControllerSystem/HurtboxControllerSystem.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/HurtboxControllerComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <catch2/catch_test_macros.hpp>

class HurtboxControllerSystemFixture
{
public:
    HurtboxControllerSystemFixture()
    {
        auto& comp = this->world.components();
        comp.registerComponent<HurtboxControllerComponent>();
        comp.registerComponent<ActiveComponent>();
    }

    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    HurtboxControllerSystem system;
};

TEST_CASE_METHOD(HurtboxControllerSystemFixture, "HurtboxControllerSystem advances frames and toggles active state",
    "[unit][hurtbox_controller_system]"
) {
    auto& comp = this->world.components();
    auto& entities = this->world.entities();

    Entity entity = entities.create();

    HurtboxControllerComponent controller;
    controller.loop = false;

    HurtboxControllerComponent::Frame frame0;
    frame0.duration = 0.1f;
    Entity hb0 = entities.create();
    comp.add<ActiveComponent>(hb0, ActiveComponent{false});
    frame0.hurtboxes.push_back(hb0);

    HurtboxControllerComponent::Frame frame1;
    frame1.duration = 0.2f;
    Entity hb1 = entities.create();
    comp.add<ActiveComponent>(hb1, ActiveComponent{false});
    frame1.hurtboxes.push_back(hb1);

    controller.frames.push_back(frame0);
    controller.frames.push_back(frame1);
    controller.currentFrame = 0;
    controller.elapsedTime = 0.f;

    comp.get<ActiveComponent>(hb0).active = true;
    controller.initialized = true;

    comp.add<HurtboxControllerComponent>(entity, std::move(controller));

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.05f};
    this->system.update(ctx);

    REQUIRE(comp.get<ActiveComponent>(hb0).active == true);
    REQUIRE(comp.get<ActiveComponent>(hb1).active == false);

    this->system.update(ctx);

    REQUIRE(comp.get<ActiveComponent>(hb0).active == false);
    REQUIRE(comp.get<ActiveComponent>(hb1).active == true);
}

TEST_CASE_METHOD(HurtboxControllerSystemFixture, "HurtboxControllerSystem loops frames when loop is true",
    "[unit][hurtbox_controller_system]"
) {
    auto& comp = this->world.components();
    auto& entities = this->world.entities();

    Entity entity = entities.create();

    HurtboxControllerComponent controller;
    controller.loop = true;

    HurtboxControllerComponent::Frame frame0;
    frame0.duration = 0.1f;
    Entity hb0 = entities.create();
    comp.add<ActiveComponent>(hb0, ActiveComponent{false});
    frame0.hurtboxes.push_back(hb0);

    HurtboxControllerComponent::Frame frame1;
    frame1.duration = 0.1f;
    Entity hb1 = entities.create();
    comp.add<ActiveComponent>(hb1, ActiveComponent{false});
    frame1.hurtboxes.push_back(hb1);

    controller.frames.push_back(frame0);
    controller.frames.push_back(frame1);
    controller.currentFrame = 0;
    controller.elapsedTime = 0.f;

    comp.get<ActiveComponent>(hb0).active = true;
    controller.initialized = true;

    comp.add<HurtboxControllerComponent>(entity, std::move(controller));

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.15f};
    this->system.update(ctx);

    REQUIRE(comp.get<ActiveComponent>(hb0).active == false);
    REQUIRE(comp.get<ActiveComponent>(hb1).active == true);

    this->system.update(ctx);

    REQUIRE(comp.get<ActiveComponent>(hb0).active == true);
    REQUIRE(comp.get<ActiveComponent>(hb1).active == false);
}

TEST_CASE_METHOD(HurtboxControllerSystemFixture, "HurtboxControllerSystem stays on last frame when not looping",
    "[unit][hurtbox_controller_system]"
) {
    auto& comp = this->world.components();
    auto& entities = this->world.entities();

    Entity entity = entities.create();

    HurtboxControllerComponent controller;
    controller.loop = false;

    HurtboxControllerComponent::Frame frame0;
    frame0.duration = 0.1f;
    Entity hb0 = entities.create();
    comp.add<ActiveComponent>(hb0, ActiveComponent{false});
    frame0.hurtboxes.push_back(hb0);

    HurtboxControllerComponent::Frame frame1;
    frame1.duration = 0.1f;
    Entity hb1 = entities.create();
    comp.add<ActiveComponent>(hb1, ActiveComponent{false});
    frame1.hurtboxes.push_back(hb1);

    controller.frames.push_back(frame0);
    controller.frames.push_back(frame1);
    controller.currentFrame = 0;
    controller.elapsedTime = 0.f;

    comp.get<ActiveComponent>(hb0).active = true;
    controller.initialized = true;

    comp.add<HurtboxControllerComponent>(entity, std::move(controller));

    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.25f};
    this->system.update(ctx);

    REQUIRE(comp.get<ActiveComponent>(hb0).active == false);
    REQUIRE(comp.get<ActiveComponent>(hb1).active == true);

    this->system.update(ctx);

    REQUIRE(comp.get<ActiveComponent>(hb0).active == false);
    REQUIRE(comp.get<ActiveComponent>(hb1).active == true);
}
