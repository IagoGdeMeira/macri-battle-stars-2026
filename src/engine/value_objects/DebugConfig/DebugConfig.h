#ifndef debug_config_h
#define debug_config_h

#include "domain/value_objects/Color/Color.h"

struct DebugConfig
{
    bool enabled = false;
    Color color = Color::WHITE();
    int layer = 0, zIndex = 20;
    bool filled = true;
};

#endif // debug_config_h
