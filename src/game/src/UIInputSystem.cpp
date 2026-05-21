#include "../include/UIInputSystem/UIInputSystem.h"

#include "../events/UIActionEvent.h"

#include "../../domain/components/UIActionComponent.h"
#include "../../domain/components/UIFocusable.h"
#include "../../domain/components/UITransform.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <limits>

UIInputSystem::UIInputSystem(EventBus& bus) : bus(bus)
{ 
    bus.subscribe<DigitalInputEvent>([this](const DigitalInputEvent& e)
    { this->digitalEvents.push_back(e); });
    
    bus.subscribe<MousePositionEvent>([this](const MousePositionEvent& e)
    { this->mousePosition = e.position; });
}

void UIInputSystem::update(UpdateContext& ctx)
{
    for (const auto& event : this->digitalEvents)
    {
        if (!event.pressed) continue;

        using Type = InputSource::Type;
        switch (event.source.type())
        {
            case Type::Keyboard:
                this->navigateKeyboard(ctx, static_cast<KeyCode>(event.source.code()));
                break;
            case Type::Mouse:
                this->navigateMouse(ctx, static_cast<MouseButton>(event.source.code()));
                break;
            case Type::Gamepad:
                this->navigateGamepad(ctx, static_cast<GamepadButton>(event.source.code()));
                break;
        }
    }
    this->digitalEvents.clear();
}

void UIInputSystem::navigate(UpdateContext& ctx, Direction dir)
{
    if (!this->focusedEntity.has_value())
    {
        auto& comp = ctx.world.components();
        auto focusables = View<UIFocusable, UITransform>(comp);
        for (auto [entity, focusable, transform] : focusables)
        {
            if (!focusable.canFocus) continue;
            
            this->focusedEntity = entity;
            return;
        }
        return;
    }

    std::optional<Entity> next = this->findClosest(ctx, this->focusedEntity.value(), dir);
    if (next.has_value()) this->focusedEntity = next.value();
}

void UIInputSystem::navigateKeyboard(UpdateContext& ctx, KeyCode key)
{
    switch (key)
    {
        case KeyCode::Up: case KeyCode::W:          this->navigate(ctx, {0, -1}); break;
        case KeyCode::Down: case KeyCode::S:        this->navigate(ctx, {0, 1}); break;
        case KeyCode::Left: case KeyCode::A:        this->navigate(ctx, {-1, 0}); break;
        case KeyCode::Right: case KeyCode::D:       this->navigate(ctx, {1, 0}); break;
        case KeyCode::Enter: case KeyCode::Space:   this->activate(ctx); break;
        default: break;
    }
}

void UIInputSystem::navigateGamepad(UpdateContext& ctx, GamepadButton button)
{
    switch (button)
    {
        case GamepadButton::DpadUp:     this->navigate(ctx, {0, -1}); break;
        case GamepadButton::DpadDown:   this->navigate(ctx, {0, 1}); break;
        case GamepadButton::DpadLeft:   this->navigate(ctx, {-1, 0}); break;
        case GamepadButton::DpadRight:  this->navigate(ctx, {1, 0}); break;
        case GamepadButton::A:          this->activate(ctx); break;
        default: break;
    }
}

void UIInputSystem::navigateMouse(UpdateContext& ctx, MouseButton button)
{
    if (button != MouseButton::Left) return;

    auto& comp = ctx.world.components();
    auto focusables = View<UIFocusable, UITransform>(comp);
    for (auto [entity, focusable, transform] : focusables)
    {
        if (!focusable.canFocus) continue;
        const Rectangle& rect = transform.rect;
        
        if (this->mousePosition.x < rect.position.x) continue;
        if (this->mousePosition.x > rect.position.x + rect.size.width) continue;
        if (this->mousePosition.y < rect.position.y) continue;
        if (this->mousePosition.y > rect.position.y + rect.size.height) continue;

        if (comp.has<UIActionComponent>(entity)) this->bus.emit<UIActionEvent>(UIActionEvent{ entity });
        break;
    }
}

void UIInputSystem::activate(UpdateContext& ctx)
{
    if (!this->focusedEntity.has_value()) return;

    if (ctx.world.components().has<UIActionComponent>(this->focusedEntity.value()))
    { this->bus.emit<UIActionEvent>(UIActionEvent{ this->focusedEntity.value() }); }
}

std::optional<Entity> UIInputSystem::findClosest(UpdateContext& ctx, Entity current, Direction dir) const
{
    auto& comp = ctx.world.components();
    if (!comp.has<UITransform>(current)) return std::nullopt;

    const auto& uiTransform = comp.get<UITransform>(current);
    Position cur =
    {
        uiTransform.rect.position.x + uiTransform.rect.size.width * 0.5f,
        uiTransform.rect.position.y + uiTransform.rect.size.height * 0.5f
    };

    std::optional<Entity> best = std::nullopt;
    float bestScore = std::numeric_limits<float>::max();

    auto focusables = View<UIFocusable, UITransform>(comp);
    for (auto [entity, focusable, transform] : focusables)
    {
        if (entity == current || !focusable.canFocus) continue;

        Position next =
        {
            transform.rect.position.x + transform.rect.size.width * 0.5f,
            transform.rect.position.y + transform.rect.size.height * 0.5f
        };
        Position delta { next.x - cur.x, next.y - cur.y };

        if ((dir.dx > 0 && delta.x <= 0)) continue;
        if ((dir.dx < 0 && delta.x >= 0)) continue;
        if ((dir.dy > 0 && delta.y <= 0)) continue;
        if ((dir.dy < 0 && delta.y >= 0)) continue;

        float dist = delta.x * delta.x + delta.y * delta.y;
        if (dist >= bestScore) continue;
        
        bestScore = dist;
        best = entity;
    }
    return best;
}
