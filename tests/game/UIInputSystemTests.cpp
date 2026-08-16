#include "game/include/UIInputSystem/UIInputSystem.h"

#include "domain/components/TransformComponent.h"
#include "domain/components/UIActionComponent.h"
#include "domain/components/UIFocusable.h"
#include "domain/components/UIRectComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include "game/events/UIActionEvent.h"

#include <catch2/catch_test_macros.hpp>

#include <vector>

class UIInputSystemFixture
{
public:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    UIInputSystem system;

    std::vector<uint32_t> activatedEntityIds;

    UIInputSystemFixture() : system(this->bus)
    {
        auto& components = this->world.components();
        components.registerComponent<UIFocusable>();
        components.registerComponent<TransformComponent>();
        components.registerComponent<UIRectComponent>();
        components.registerComponent<UIActionComponent>();

        this->bus.subscribe<UIActionEvent>([this](const UIActionEvent& event)
        { this->activatedEntityIds.push_back(event.entity.id); });
    }

    Entity createButtonEntity(uint32_t id, const Rectangle& rect)
    {
        Entity entity{id};

        auto& components = this->world.components();
        components.add<UIFocusable>(entity, UIFocusable{true});

        TransformComponent tc;
        tc.position = rect.position;
        components.add<TransformComponent>(entity, tc);

        UIRectComponent urc;
        urc.size = rect.size;
        components.add<UIRectComponent>(entity, urc);

        components.add<UIActionComponent>(entity, UIActionComponent{[]() {}});

        return entity;
    }

    UpdateContext makeContext(float dt = 1.f / 60.f)
    { return UpdateContext{this->world, this->bus, this->commandBuffer, dt}; }
};

TEST_CASE_METHOD(UIInputSystemFixture, "UIInputSystem accepts input from any player when no filter is set",
    "[unit][ui_input_system]"
) {
    Entity button = this->createButtonEntity(1, Rectangle{Position{10.f, 20.f}, Dimension2D{140.f, 40.f}});

    this->bus.emit<MousePositionEvent>(MousePositionEvent{Position{20.f, 30.f}});
    this->bus.emit<DigitalInputEvent>(InputSource::mouse(MouseButton::Left), 99, true);

    UpdateContext ctx = this->makeContext();
    this->system.update(ctx);

    REQUIRE(this->activatedEntityIds.size() == 1);
    REQUIRE(this->activatedEntityIds[0] == button.id);
}

TEST_CASE_METHOD(UIInputSystemFixture, "UIInputSystem ignores input from players not in allowed filter",
    "[unit][ui_input_system]"
) {
    this->createButtonEntity(7, Rectangle{Position{10.f, 20.f}, Dimension2D{140.f, 40.f}});

    this->system.setAllowedPlayer(1);

    this->bus.emit<MousePositionEvent>(MousePositionEvent{Position{20.f, 30.f}});
    this->bus.emit<DigitalInputEvent>(InputSource::mouse(MouseButton::Left), 2, true);

    UpdateContext ctx = this->makeContext();
    this->system.update(ctx);

    REQUIRE(this->activatedEntityIds.empty());
}

TEST_CASE_METHOD(UIInputSystemFixture, "UIInputSystem processes keyboard navigation and activation for allowed player",
    "[unit][ui_input_system]"
) {
    Entity button = this->createButtonEntity(42, Rectangle{Position{0.f, 0.f}, Dimension2D{100.f, 30.f}});

    this->system.setAllowedPlayer(7);

    this->bus.emit<DigitalInputEvent>(InputSource::keyboard(KeyCode::Down), 3, true);
    this->bus.emit<DigitalInputEvent>(InputSource::keyboard(KeyCode::Enter), 3, true);
    this->bus.emit<DigitalInputEvent>(InputSource::keyboard(KeyCode::Down), 7, true);
    this->bus.emit<DigitalInputEvent>(InputSource::keyboard(KeyCode::Enter), 7, true);

    UpdateContext ctx = this->makeContext();
    this->system.update(ctx);

    REQUIRE(this->activatedEntityIds.size() == 1);
    REQUIRE(this->activatedEntityIds[0] == button.id);
}
