#ifndef input_binding_h
#define input_binding_h

#include "../InputSource/InputSource.h"

#include "../../domain/include/InputAction/InputAction.h"

#include <unordered_map>

struct InputBinding
{ std::unordered_map<InputSource, InputAction, InputSource::Hash> keyMap; };

#endif // input_binding_h
