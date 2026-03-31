#ifndef input_buffer_component_h
#define input_buffer_component_h

#include "../include/InputAction/InputAction.h"

#include <deque>

struct InputBufferEntry
{
    InputAction action;
    float time;
};

class InputBufferComponent
{
    std::deque<InputBufferEntry> buffer;
};

#endif // input_buffer_component_h
