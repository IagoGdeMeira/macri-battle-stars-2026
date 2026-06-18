#ifndef input_component_h
#define input_component_h

#include "../value_objects/InputAction/InputAction.h"

#include <unordered_map>

struct InputState { bool pressed = false; float heldTime = 0.0f; };

struct InputComponent { std::unordered_map<InputAction, InputState> actions; };

#endif // input_component_h
