#ifndef input_component_h
#define input_component_h

#include "InputAction/InputAction.h"

#include <unordered_map>

struct InputComponent
{
    struct State { bool pressed = false; float heldTime = 0.f; };

    std::unordered_map<InputAction, State> actions;
};

#endif // input_component_h
