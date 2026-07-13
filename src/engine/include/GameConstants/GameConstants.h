#ifndef game_constants_h
#define game_constants_h

#include "../../domain/value_objects/Geometry/Geometry.h"

struct GameConstants
{
    static constexpr Dimension2D VIRTUAL_SIZE{800.f, 600.f};
    static constexpr const char* DEFAULT_FONT = "assets/fonts/default.ttf";
    
    static constexpr const char* DEFAULT_MAP_PATH = "assets/maps/stage1.json";
    static constexpr const char* DEFAULT_INPUT_BINDINGS_PATH = "assets/inputs/game_bindings.json";
    static constexpr const char* DEFAULT_COMBOS_PATH = "assets/combos/default_combos.json";
    static constexpr const char* DEFAULT_TRIGGERS_PATH = "assets/triggers/default_triggers.json";
    static constexpr const char* DEFAULT_CHARACTER_DEF_PATH = "assets/characters/reddie_beta.json";

    static constexpr int TARGET_FPS = 60;
};

#endif // game_constants_h
