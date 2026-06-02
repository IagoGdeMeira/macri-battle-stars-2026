#ifndef platform_event_h
#define platform_event_h

#include "../../domain/include/Geometry/Geometry.h"

#include <cstdint>

class PlatformEvent
{
public:
    enum class Type { Keyboard, Mouse, Gamepad, Quit, WindowResized };
    virtual ~PlatformEvent() = default;
    virtual Type type() const = 0;
};

class KeyboardEvent : public PlatformEvent
{
public:
    uint32_t keyCode;
    bool pressed;

    KeyboardEvent(uint32_t key, bool press) : keyCode(key), pressed(press) {}
    Type type() const override { return Type::Keyboard; }
};

class MouseButtonEvent : public PlatformEvent
{
public:
    uint32_t button;
    bool pressed;

    MouseButtonEvent(uint32_t btn, bool press) : button(btn), pressed(press) {}
    Type type() const override { return Type::Mouse; }
};

class MouseMotionEvent : public PlatformEvent
{
public:
    Position delta {0.f, 0.f};
    Position absolute {0.f, 0.f};

    MouseMotionEvent(Position delta, Position absolute) : delta(delta), absolute(absolute) {}
    Type type() const override { return Type::Mouse; }
};

class GamepadButtonEvent : public PlatformEvent
{
public:
    uint32_t button;
    bool pressed;

    GamepadButtonEvent(uint32_t btn, bool press) : button(btn), pressed(press) {}
    Type type() const override { return Type::Gamepad; }
};

class GamepadAxisEvent : public PlatformEvent
{
public:
    uint32_t axis;
    float value;

    GamepadAxisEvent(uint32_t ax, float val) : axis(ax), value(val) {}
    Type type() const override { return Type::Gamepad; }
};

class QuitPlatformEvent : public PlatformEvent
{
public:
    QuitPlatformEvent() {}
    Type type() const override { return Type::Quit; }
};

class WindowResizedPlatformEvent : public PlatformEvent
{
public:
    Dimension2D size;

    WindowResizedPlatformEvent(Dimension2D sz) : size(sz) {}
    Type type() const override { return Type::WindowResized; }
};

#endif // platform_event_h
