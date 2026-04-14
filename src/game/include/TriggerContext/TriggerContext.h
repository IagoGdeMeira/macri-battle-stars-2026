#ifndef trigger_context_h
#define trigger_context_h

#include "../TriggerBinding/TriggerBinding.h"

#include <unordered_map>

struct TriggerContext
{ std::unordered_map<InputAction, std::vector<TriggerId>> bindings; };

#endif // trigger_context_h
