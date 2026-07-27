#ifndef debug_config_h
#define debug_config_h

#include "domain/value_objects/Color/Color.h"

struct DebugConfig
{
    bool enabled = false;
    Color color = Color::WHITE();
    int zIndex = 0, layer = 0;
};

#endif // debug_config_h
