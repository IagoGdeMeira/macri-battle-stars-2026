#ifndef sdl_key_translator
#define sdl_key_translator

#include "../../engine/include/GamepadButton/GamepadButton.h" 
#include "../../engine/include/KeyCode/KeyCode.h"
#include "../../engine/include/MouseButton/MouseButton.h"

class SDLKeyTranslator
{
public:
    static KeyCode toKeyCode(int key);
    static int fromKeyCode(KeyCode key);

    static MouseButton toMouseButton(int sdlButton);
    static int fromMouseButton(MouseButton button);

    static GamepadButton toGamepadButton(int sdlButton);
    static int fromGamepadButton(GamepadButton button);
};

#endif // sdl_key_translator
