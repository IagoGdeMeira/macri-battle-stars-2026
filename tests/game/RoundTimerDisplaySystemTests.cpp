#include "game/include/RoundTimerDisplaySystem/RoundTimerDisplaySystem.h"

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
        Entity e = world.entities().create();
        comp.add<RoundTimerTag>(e, RoundTimerTag{});
        comp.add<UITextComponent>(e, UITextComponent{nullptr, "", Color::WHITE(), true, 24.f});
        return e;
    }
};

TEST_CASE_METHOD(RoundTimerDisplaySystemFixture, "Display updates text from event", "[unit][round_timer_display]")
{
    Entity timerEntity = this->createTimerEntity();
    this->bus.emit<RoundTimeEvent>(RoundTimeEvent{99.5f});
    this->system.update(this->ctx);
    auto& text = this->world.components().get<UITextComponent>(timerEntity);
    REQUIRE(text.text == "99");
}
