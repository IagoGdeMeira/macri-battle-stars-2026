#include "../../src/game/include/UIInputSystem/UIInputSystem.h"

#include "../../src/domain/components/UIActionComponent.h"
#include "../../src/domain/components/UIFocusable.h"
#include "../../src/domain/components/UITransform.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"

#include "../../src/game/events/UIActionEvent.h"

#include <catch2/catch_test_macros.hpp>

#include <vector>

class UIInputSystemFixture
{
public:
    UIInputSystemFixture() : system(this->bus)
    {
        auto& components = this->world.components();
        components.registerComponent<UIFocusable>();
        components.registerComponent<UITransform>();
        components.registerComponent<UIActionComponent>();

        this->bus.subscribe<UIActionEvent>([this](const UIActionEvent& event)
        { this->activatedEntityIds.push_back(event.entity.id); });
    }

    Entity createButtonEntity(uint32_t id, const Rectangle& rect)
    {
        Entity entity{id};

        this->world.components().add<UIFocusable>(entity, UIFocusable{true});

        UITransform transform;
        transform.rect = rect;
        this->world.components().add<UITransform>(entity, transform);

        this->world.components().add<UIActionComponent>(entity, UIActionComponent{[]() {}});

        return entity;
    }

    UpdateContext makeContext(float dt = 1.0f / 60.0f)
    { return UpdateContext{this->world, this->bus, this->commandBuffer, dt}; }

    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    UIInputSystem system;

    std::vector<uint32_t> activatedEntityIds;
};

TEST_CASE_METHOD(UIInputSystemFixture, "UIInputSystem accepts input from any player when no filter is set",
    "[unit][ui_input_system]"
) {
    Entity button = this->createButtonEntity(1, Rectangle{Position{10.0f, 20.0f}, Dimension2D{140.0f, 40.0f}});

    this->bus.emit<MousePositionEvent>(MousePositionEvent{Position{20.0f, 30.0f}});
    this->bus.emit<DigitalInputEvent>(InputSource::mouse(MouseButton::Left), 99, true);

    UpdateContext ctx = this->makeContext();
    this->system.update(ctx);

    REQUIRE(this->activatedEntityIds.size() == 1);
    REQUIRE(this->activatedEntityIds[0] == button.id);
}

TEST_CASE_METHOD(UIInputSystemFixture, "UIInputSystem ignores input from players not in allowed filter",
    "[unit][ui_input_system]"
) {
    this->createButtonEntity(7, Rectangle{Position{10.0f, 20.0f}, Dimension2D{140.0f, 40.0f}});

    this->system.setAllowedPlayer(1);

    this->bus.emit<MousePositionEvent>(MousePositionEvent{Position{20.0f, 30.0f}});
    this->bus.emit<DigitalInputEvent>(InputSource::mouse(MouseButton::Left), 2, true);

    UpdateContext ctx = this->makeContext();
    this->system.update(ctx);

    REQUIRE(this->activatedEntityIds.empty());
}

TEST_CASE_METHOD(UIInputSystemFixture, "UIInputSystem processes keyboard navigation and activation for allowed player",
    "[unit][ui_input_system]"
) {
    Entity button = this->createButtonEntity(42, Rectangle{Position{0.0f, 0.0f}, Dimension2D{100.0f, 30.0f}});

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
