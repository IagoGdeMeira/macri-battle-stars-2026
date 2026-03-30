#ifndef sdl_key_translator
#define sdl_key_translator

#include "../../engine/include/KeyCode/KeyCode.h"

class SDLKeyTranslator
{
public:
    static KeyCode toKeyCode(int sdlKey);
};

#endif // sdl_key_translator
