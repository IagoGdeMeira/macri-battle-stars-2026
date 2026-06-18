#ifndef input_buffer_component_h
#define input_buffer_component_h

#include "../value_objects/InputAction/InputAction.h"

#include <deque>

struct InputBufferEntry { InputAction action; float time; };

struct InputBufferComponent { std::deque<InputBufferEntry> buffer; };

#endif // input_buffer_component_h
