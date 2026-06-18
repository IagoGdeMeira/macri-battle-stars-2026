#ifndef input_mapper_h
#define input_mapper_h

#include "../GamepadButton/GamepadButton.h"
#include "../InputSource/InputSource.h"
#include "../KeyCode/KeyCode.h"
#include "../MouseButton/MouseButton.h"

#include "../../domain/value_objects/InputAction/InputAction.h"

#include <string>

class InputMapper
{
public:
    static InputAction stringToAction(const std::string& actionStr);
    static std::string actionToString(InputAction action);

    static KeyCode stringToKey(const std::string& keyStr);
    static std::string keyToString(KeyCode keyCode);

    static MouseButton stringToMouseButton(const std::string& buttonStr);
    static std::string mouseButtonToString(MouseButton button);

    static GamepadButton stringToGamepadButton(const std::string& buttonStr);
    static std::string gamepadButtonToString(GamepadButton button);

    static InputSource stringToSource(const std::string& sourceStr);
};

#endif // input_mapper_h
