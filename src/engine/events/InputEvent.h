#ifndef input_event_h
#define input_event_h

#include "../include/InputSource/InputSource.h"

#include <cstdint>

class InputEvent
{
public:
    virtual ~InputEvent() = default;

    InputSource source;
    uint32_t playerId;

protected:
    InputEvent(InputSource src, uint32_t player) : source(src), playerId(player) {}
};

class DigitalInputEvent : public InputEvent
{
public:
    bool pressed;

    DigitalInputEvent(InputSource src, uint32_t player, bool pressed) :
        InputEvent(src, player), pressed(pressed) {}
};

class AnalogInputEvent : public InputEvent
{
public:
    float value;

    AnalogInputEvent(InputSource src, uint32_t player, float value) :
        InputEvent(src, player), value(value) {}
};

#endif // input_event_h
