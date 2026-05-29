#ifndef game_constants_h
#define game_constants_h

#include "../../../domain/include/Geometry/Geometry.h"

struct GameConstants
{
    static constexpr Dimension2D VIRTUAL_SIZE{ 800.f, 600.f };
    static constexpr const char* DEFAULT_FONT = "assets/fonts/default.ttf";
    
};

#endif // game_constants_h
