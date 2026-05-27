#ifndef game_settings_h
#define game_settings_h

#include "../Viewport/Viewport.h"

#include "../../../domain/include/Geometry/Geometry.h"

class GameSettings
{
public:
    struct Volume { int master = 100, music = 80, sfx = 100; };
    struct Screen { bool fullscreen = false; Dimension2D size {800.f, 600.f}; };

    Volume volume;
    Screen screen;
    
    Viewport viewport {0, 0, 800, 600};
    static constexpr Dimension2D VIRTUAL_SIZE {800.f, 600.f};
};

#endif // game_settings_h
