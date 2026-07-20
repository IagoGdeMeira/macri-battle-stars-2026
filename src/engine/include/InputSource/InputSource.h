#ifndef input_source_h
#define input_source_h

#include "GamepadButton/GamepadButton.h"
#include "KeyCode/KeyCode.h"
#include "MouseButton/MouseButton.h"

#include <cstdint>
#include <functional>

class InputSource
{
public:
    enum class Type : uint8_t { Keyboard, Mouse, Gamepad };

    static InputSource keyboard(KeyCode key)
    { return InputSource(Type::Keyboard, static_cast<uint32_t>(key)); }
    
    static InputSource mouse(MouseButton button)
    { return InputSource(Type::Mouse, static_cast<uint32_t>(button)); }
    
    static InputSource gamepad(GamepadButton button)
    { return InputSource(Type::Gamepad, static_cast<uint32_t>(button)); }

    Type type() const { return this->sourceType; }
    uint32_t code() const { return this->sourceCode; }

    bool operator==(const InputSource &other) const
    { return this->sourceType == other.sourceType && this->sourceCode == other.sourceCode; }

    bool operator!=(const InputSource& other) const { return !(*this == other); }

    struct Hash
    {
        std::size_t operator()(const InputSource& src) const
        { return std::hash<uint32_t>()(static_cast<uint32_t>(src.sourceType) ^ (src.sourceCode << 8)); }
    };

private:
    InputSource(Type t, uint32_t c) : sourceType(t), sourceCode(c) {}

    Type sourceType;
    uint32_t sourceCode;
};

#endif // input_source_h
