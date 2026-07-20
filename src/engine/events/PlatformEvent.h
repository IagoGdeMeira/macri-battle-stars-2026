#ifndef platform_event_h
#define platform_event_h

#include "domain/value_objects/Geometry/Geometry.h"

#include <cstdint>
#include <memory>

class PlatformEvent
{
public:
    enum class Type { Keyboard, Mouse, Gamepad, Quit, WindowResized };
    
    virtual ~PlatformEvent() = default;

    virtual Type type() const = 0;
    virtual std::unique_ptr<PlatformEvent> clone() const = 0;
};

class KeyboardEvent : public PlatformEvent
{
public:
    uint32_t keyCode;
    bool pressed;

    KeyboardEvent(uint32_t key, bool press) : keyCode(key), pressed(press) {}

    Type type() const override { return Type::Keyboard; }
    std::unique_ptr<PlatformEvent> clone() const override { return std::make_unique<KeyboardEvent>(*this); }
};

class MouseButtonEvent : public PlatformEvent
{
public:
    uint32_t button;
    bool pressed;

    MouseButtonEvent(uint32_t btn, bool press) : button(btn), pressed(press) {}

    Type type() const override { return Type::Mouse; }
    std::unique_ptr<PlatformEvent> clone() const override { return std::make_unique<MouseButtonEvent>(*this); }
};

class MouseMotionEvent : public PlatformEvent
{
public:
    Position delta {0.f, 0.f};
    Position absolute {0.f, 0.f};

    MouseMotionEvent(Position delta, Position absolute) : delta(delta), absolute(absolute) {}

    Type type() const override { return Type::Mouse; }
    std::unique_ptr<PlatformEvent> clone() const override { return std::make_unique<MouseMotionEvent>(*this); }
};

class GamepadButtonEvent : public PlatformEvent
{
public:
    uint32_t button;
    bool pressed;

    GamepadButtonEvent(uint32_t btn, bool press) : button(btn), pressed(press) {}

    Type type() const override { return Type::Gamepad; }
    std::unique_ptr<PlatformEvent> clone() const override { return std::make_unique<GamepadButtonEvent>(*this); }
};

class GamepadAxisEvent : public PlatformEvent
{
public:
    uint32_t axis;
    float value;

    GamepadAxisEvent(uint32_t ax, float val) : axis(ax), value(val) {}

    Type type() const override { return Type::Gamepad; }
    std::unique_ptr<PlatformEvent> clone() const override { return std::make_unique<GamepadAxisEvent>(*this); }
};

class QuitPlatformEvent : public PlatformEvent
{
public:
    QuitPlatformEvent() {}
    
    Type type() const override { return Type::Quit; }
    std::unique_ptr<PlatformEvent> clone() const override { return std::make_unique<QuitPlatformEvent>(*this); }
};

class WindowResizedPlatformEvent : public PlatformEvent
{
public:
    Dimension2D size;

    WindowResizedPlatformEvent(Dimension2D sz) : size(sz) {}

    Type type() const override { return Type::WindowResized; }
    std::unique_ptr<PlatformEvent> clone() const override { return std::make_unique<WindowResizedPlatformEvent>(*this); }
};

#endif // platform_event_h
