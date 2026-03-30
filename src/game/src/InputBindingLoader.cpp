#include "../include/InputBindingLoader/InputBindingLoader.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

using json = nlohmann::json;

void InputBindingLoader::load(const std::string& path)
{
    std::ifstream file(path);
    json data = json::parse(file);

    InputContext context;

    for (const auto& player : data["players"])
    {
        PlayerId playerId = player["id"];
        InputBinding binding;

        for (auto& [action, key] : player["bindings"].items())
        {
            auto keyCode = InputBindingLoader::stringToKeyCode(key);
            auto inputAction = InputBindingLoader::stringToInputAction(action);
            binding.keyMap[keyCode] = inputAction;
        }

        context.bindings[playerId] = binding;
    }
}

KeyCode InputBindingLoader::stringToKeyCode(const std::string& keyStr)
{
    if (keyStr == "A") return KeyCode::A;
    if (keyStr == "B") return KeyCode::B;
    if (keyStr == "C") return KeyCode::C;
    if (keyStr == "D") return KeyCode::D;
    if (keyStr == "E") return KeyCode::E;
    if (keyStr == "F") return KeyCode::F;
    if (keyStr == "G") return KeyCode::G;
    if (keyStr == "H") return KeyCode::H;
    if (keyStr == "I") return KeyCode::I;
    if (keyStr == "J") return KeyCode::J;
    if (keyStr == "K") return KeyCode::K;
    if (keyStr == "L") return KeyCode::L;
    if (keyStr == "M") return KeyCode::M;
    if (keyStr == "N") return KeyCode::N;
    if (keyStr == "O") return KeyCode::O;
    if (keyStr == "P") return KeyCode::P;
    if (keyStr == "Q") return KeyCode::Q;
    if (keyStr == "R") return KeyCode::R;
    if (keyStr == "S") return KeyCode::S;
    if (keyStr == "T") return KeyCode::T;
    if (keyStr == "U") return KeyCode::U;
    if (keyStr == "V") return KeyCode::V;
    if (keyStr == "W") return KeyCode::W;
    if (keyStr == "X") return KeyCode::X;
    if (keyStr == "Y") return KeyCode::Y;
    if (keyStr == "Z") return KeyCode::Z;
    if (keyStr == "Num0") return KeyCode::Num0;
    if (keyStr == "Num1") return KeyCode::Num1;
    if (keyStr == "Num2") return KeyCode::Num2;
    if (keyStr == "Num3") return KeyCode::Num3;
    if (keyStr == "Num4") return KeyCode::Num4;
    if (keyStr == "Num5") return KeyCode::Num5;
    if (keyStr == "Num6") return KeyCode::Num6;
    if (keyStr == "Num7") return KeyCode::Num7;
    if (keyStr == "Num8") return KeyCode::Num8;
    if (keyStr == "Num9") return KeyCode::Num9;
    if (keyStr == "Escape") return KeyCode::Escape;
    if (keyStr == "LControl") return KeyCode::LControl;
    if (keyStr == "LShift") return KeyCode::LShift;
    if (keyStr == "LAlt") return KeyCode::LAlt;
    if (keyStr == "LSystem") return KeyCode::LSystem;
    if (keyStr == "RControl") return KeyCode::RControl;
    if (keyStr == "RShift") return KeyCode::RShift;
    if (keyStr == "RAlt") return KeyCode::RAlt;
    if (keyStr == "RSystem") return KeyCode::RSystem;
    if (keyStr == "Menu") return KeyCode::Menu;
    if (keyStr == "LBracket") return KeyCode::LBracket;
    if (keyStr == "RBracket") return KeyCode::RBracket;
    if (keyStr == "Semicolon") return KeyCode::Semicolon;
    if (keyStr == "Comma") return KeyCode::Comma;
    if (keyStr == "Period") return KeyCode::Period;
    if (keyStr == "Quote") return KeyCode::Quote;
    if (keyStr == "Slash") return KeyCode::Slash;
    if (keyStr == "Backslash") return KeyCode::Backslash;
    if (keyStr == "Tilde") return KeyCode::Tilde;
    if (keyStr == "Equal") return KeyCode::Equal;
    if (keyStr == "Hyphen") return KeyCode::Hyphen;
    if (keyStr == "Space") return KeyCode::Space;
    if (keyStr == "Enter") return KeyCode::Enter;
    if (keyStr == "Backspace") return KeyCode::Backspace;
    if (keyStr == "Tab") return KeyCode::Tab;
    if (keyStr == "PageUp") return KeyCode::PageUp;
    if (keyStr == "PageDown") return KeyCode::PageDown;
    if (keyStr == "End") return KeyCode::End;
    if (keyStr == "Home") return KeyCode::Home;
    if (keyStr == "Insert") return KeyCode::Insert;
    if (keyStr == "Delete") return KeyCode::Delete;
    if (keyStr == "Add") return KeyCode::Add;
    if (keyStr == "Subtract") return KeyCode::Subtract;
    if (keyStr == "Multiply") return KeyCode::Multiply;
    if (keyStr == "Divide") return KeyCode::Divide;
    if (keyStr == "Left") return KeyCode::Left;
    if (keyStr == "Right") return KeyCode::Right;
    if (keyStr == "Up") return KeyCode::Up;
    if (keyStr == "Down") return KeyCode::Down;
    if (keyStr == "Numpad0") return KeyCode::Numpad0;
    if (keyStr == "Numpad1") return KeyCode::Numpad1;
    if (keyStr == "Numpad2") return KeyCode::Numpad2;
    if (keyStr == "Numpad3") return KeyCode::Numpad3;
    if (keyStr == "Numpad4") return KeyCode::Numpad4;
    if (keyStr == "Numpad5") return KeyCode::Numpad5;
    if (keyStr == "Numpad6") return KeyCode::Numpad6;
    if (keyStr == "Numpad7") return KeyCode::Numpad7;
    if (keyStr == "Numpad8") return KeyCode::Numpad8;
    if (keyStr == "Numpad9") return KeyCode::Numpad9;
    if (keyStr == "F1") return KeyCode::F1;
    if (keyStr == "F2") return KeyCode::F2;
    if (keyStr == "F3") return KeyCode::F3;
    if (keyStr == "F4") return KeyCode::F4;
    if (keyStr == "F5") return KeyCode::F5;
    if (keyStr == "F6") return KeyCode::F6;
    if (keyStr == "F7") return KeyCode::F7;
    if (keyStr == "F8") return KeyCode::F8;
    if (keyStr == "F9") return KeyCode::F9;
    if (keyStr == "F10") return KeyCode::F10;
    if (keyStr == "F11") return KeyCode::F11;
    if (keyStr == "F12") return KeyCode::F12;
    if (keyStr == "F13") return KeyCode::F13;
    if (keyStr == "F14") return KeyCode::F14;
    if (keyStr == "F15") return KeyCode::F15;
    if (keyStr == "Pause") return KeyCode::Pause;

    return KeyCode::Unknown;
}

InputAction InputBindingLoader::stringToInputAction(const std::string& actionStr)
{
    if (actionStr == "MoveUp") return InputAction::MoveUp;
    if (actionStr == "MoveDown") return InputAction::MoveDown;
    if (actionStr == "MoveLeft") return InputAction::MoveLeft;
    if (actionStr == "MoveRight") return InputAction::MoveRight;
    if (actionStr == "Attack") return InputAction::Attack;
    if (actionStr == "Defend") return InputAction::Defend;

    return InputAction::None;
}
