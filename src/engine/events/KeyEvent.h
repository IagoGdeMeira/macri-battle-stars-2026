#ifndef key_event_h
#define key_event_h

#include "../include/KeyCode/KeyCode.h"

#include <cstdint>

using PlayerId = uint32_t;

struct KeyEvent
{
    KeyCode key;
    bool pressed;
    PlayerId player = 0;
};

#endif // key_event_h
