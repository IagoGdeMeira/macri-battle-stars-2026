#ifndef input_binding_h
#define input_binding_h

#include "../KeyCode/KeyCode.h"

#include "../../domain/include/InputAction/InputAction.h"

#include <unordered_map>

struct InputBinding
{ std::unordered_map<KeyCode, InputAction> keyMap; };

#endif // input_binding_h
