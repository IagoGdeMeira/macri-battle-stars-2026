#ifndef input_mapper_h
#define input_mapper_h

#include "../KeyCode/KeyCode.h"

#include "../../domain/include/InputAction/InputAction.h"

#include <string>

class InputMapper
{
public:
    static InputAction stringToAction(const std::string& actionStr);
    static KeyCode stringToKey(const std::string& keyStr);

    static std::string actionToString(InputAction action);
    static std::string keyToString(KeyCode keyCode);
};

#endif // input_mapper_h
