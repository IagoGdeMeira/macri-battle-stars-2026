#ifndef sdl_key_translator
#define sdl_key_translator

#include "../../engine/include/GamepadButton/GamepadButton.h" 
#include "../../engine/include/KeyCode/KeyCode.h"
#include "../../engine/include/MouseButton/MouseButton.h"

class SDLKeyTranslator
{
public:
    static GamepadButton toGamepadButton(int sdlButton);
    static KeyCode toKeyCode(int sdlKey);
    static MouseButton toMouseButton(int sdlButton);
};

#endif // sdl_key_translator
