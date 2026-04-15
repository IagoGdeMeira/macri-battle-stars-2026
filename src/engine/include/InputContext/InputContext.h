#ifndef input_context_h
#define input_context_h

#include "../InputBinding/InputBinding.h"

#include <cstdint>
#include <unordered_map>

struct InputContext { std::unordered_map<uint32_t, InputBinding> bindings; };

#endif // input_context_h
