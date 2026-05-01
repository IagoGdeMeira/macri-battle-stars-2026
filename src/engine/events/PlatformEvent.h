#ifndef platform_event_h
#define platform_event_h

#include <cstdint>

class PlatformEvent
{
public:
    enum class Type { Keyboard, Mouse, Gamepad, Quit };
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
    float deltaX;
    float deltaY;

    MouseMotionEvent(float dx, float dy) : deltaX(dx), deltaY(dy) {}
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

#endif // platform_event_h
