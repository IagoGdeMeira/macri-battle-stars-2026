#ifndef input_binding_loader_h
#define input_binding_loader_h

#include "../../engine/include/InputContext/InputContext.h"

#include <string>

class InputBindingLoader
{
public:
    static void load(const std::string& path);

    static KeyCode stringToKeyCode(const std::string& keyStr);
    static InputAction stringToInputAction(const std::string& actionStr);
};

#endif // input_binding_loader_h
