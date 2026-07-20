#ifndef sdl_key_translator
#define sdl_key_translator

#include "engine/value_objects/GamepadButton/GamepadButton.h" 
#include "engine/value_objects/KeyCode/KeyCode.h"
#include "engine/value_objects/MouseButton/MouseButton.h"

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
