#ifndef input_context_h
#define input_context_h

#include "../InputBinding/InputBinding.h"

#include <cstdint>
#include <unordered_map>

using PlayerId = uint32_t;

struct InputContext
{ std::unordered_map<PlayerId, InputBinding> bindings; };

#endif // input_context_h
