#include "game/include/RoundTimerDisplaySystem/RoundTimerDisplaySystem.h"

#include "domain/components/ChildrenComponent.h"
#include "domain/components/RoundTimerTag.h"
#include "domain/components/UITextComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include "game/events/RoundTimeEvent.h"

#include <catch2/catch_test_macros.hpp>

class RoundTimerDisplaySystemFixture
{
public:
    RoundTimerDisplaySystemFixture()
    {
        auto& comp = this->world.components();
        comp.registerComponent<ChildrenComponent>();
        comp.registerComponent<RoundTimerTag>();
        comp.registerComponent<UITextComponent>();
    }

    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    RoundTimerDisplaySystem system{this->bus};
    UpdateContext ctx{this->world, this->bus, this->commandBuffer, 0.016f};

    Entity createTimerEntity()
    {
        auto& comp = this->world.components();
        auto& entities = this->world.entities();

        Entity parent = entities.create();
        comp.add<RoundTimerTag>(parent, RoundTimerTag{});
        comp.add<ChildrenComponent>(parent, ChildrenComponent{});

        Entity child = entities.create();
        comp.add<UITextComponent>(child, UITextComponent{nullptr, "", Color::WHITE(), true, 24.f});

        comp.get<ChildrenComponent>(parent).children.push_back(child);
        return child;
    }
};

TEST_CASE_METHOD(RoundTimerDisplaySystemFixture, "Display updates text from event", "[unit][round_timer_display]")
{
    Entity textEntity = this->createTimerEntity();
    this->bus.emit<RoundTimeEvent>(RoundTimeEvent{99.5f});
    this->system.update(this->ctx);

    auto& text = this->world.components().get<UITextComponent>(textEntity);
    REQUIRE(text.text == "99");
}
