#ifndef debug_config_h
#define debug_config_h

#include "domain/value_objects/Color/Color.h"

struct DebugConfig { bool enabled = false; Color color = Color::WHITE(); };

#endif // debug_config_h
