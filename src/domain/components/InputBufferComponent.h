#ifndef input_buffer_component_h
#define input_buffer_component_h

#include "InputAction/InputAction.h"

#include <deque>

struct InputBufferComponent
{
    struct Entry { InputAction action; float time = 0.f; };

    std::deque<Entry> buffer;
};

#endif // input_buffer_component_h
