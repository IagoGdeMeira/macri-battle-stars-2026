#include "engine/include/InputMapper/InputMapper.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("InputMapper converts known and unknown action strings", "[unit][input_mapper]")
{
    REQUIRE(InputMapper::stringToAction("MoveUp") == InputAction::MoveUp);
    REQUIRE(InputMapper::stringToAction("Punch") == InputAction::Punch);
    REQUIRE(InputMapper::stringToAction("Kick") == InputAction::Kick);
    REQUIRE(InputMapper::stringToAction("invalid") == InputAction::None);
}

TEST_CASE("InputMapper converts known and unknown key strings", "[unit][input_mapper]")
{
    REQUIRE(InputMapper::stringToKey("A") == KeyCode::A);
    REQUIRE(InputMapper::stringToKey("Escape") == KeyCode::Escape);
    REQUIRE(InputMapper::stringToKey("Numpad9") == KeyCode::Numpad9);
    REQUIRE(InputMapper::stringToKey("invalid") == KeyCode::Unknown);
}

TEST_CASE("InputMapper converts enums to strings with fallback", "[unit][input_mapper]")
{
    REQUIRE(InputMapper::actionToString(InputAction::Defend) == "Defend");
    REQUIRE(InputMapper::actionToString(InputAction::Punch) == "Punch");
    REQUIRE(InputMapper::actionToString(InputAction::Kick) == "Kick");
    REQUIRE(InputMapper::actionToString(InputAction::None) == "None");

    REQUIRE(InputMapper::keyToString(KeyCode::F12) == "F12");
    REQUIRE(InputMapper::keyToString(KeyCode::Unknown) == "Unknown");
}

TEST_CASE("InputMapper supports round-trip conversion for representative values", "[unit][input_mapper]")
{
    const auto action = InputAction::Punch;
    REQUIRE(InputMapper::stringToAction(InputMapper::actionToString(action)) == action);

    const auto kick = InputAction::Kick;
    REQUIRE(InputMapper::stringToAction(InputMapper::actionToString(kick)) == kick);

    const auto key = KeyCode::Left;
    REQUIRE(InputMapper::stringToKey(InputMapper::keyToString(key)) == key);
}

TEST_CASE("InputMapper throws for invalid source strings", "[unit][input_mapper]")
{
    REQUIRE_THROWS_AS(InputMapper::stringToSource("InvalidSource"), std::runtime_error);
    REQUIRE_THROWS_AS(InputMapper::stringToSource("Keyboard"), std::runtime_error);
    REQUIRE_THROWS_AS(InputMapper::stringToSource("Mouse.LeftButton"), std::runtime_error);
    REQUIRE_THROWS_AS(InputMapper::stringToSource("Gamepad.A"), std::runtime_error);
}

TEST_CASE("InputMapper supports round-trip conversion for InputSource", "[unit][input_mapper]")
{
    const auto source = InputSource::keyboard(KeyCode::A);
    REQUIRE(InputMapper::stringToSource(InputMapper::sourceToString(source)) == source);

    const auto mouseSource = InputSource::mouse(MouseButton::Left);
    REQUIRE(InputMapper::stringToSource(InputMapper::sourceToString(mouseSource)) == mouseSource);

    const auto gamepadSource = InputSource::gamepad(GamepadButton::X);
    REQUIRE(InputMapper::stringToSource(InputMapper::sourceToString(gamepadSource)) == gamepadSource);
}

TEST_CASE("InputMapper converts known and unknown mouse button strings", "[unit][input_mapper]")
{
    REQUIRE(InputMapper::stringToMouseButton("Left") == MouseButton::Left);
    REQUIRE(InputMapper::stringToMouseButton("Right") == MouseButton::Right);
    REQUIRE(InputMapper::stringToMouseButton("Middle") == MouseButton::Middle);
    REQUIRE(InputMapper::stringToMouseButton("invalid") == MouseButton::Unknown);
}

TEST_CASE("InputMapper converts known and unknown gamepad button strings", "[unit][input_mapper]")
{
    REQUIRE(InputMapper::stringToGamepadButton("A") == GamepadButton::A);
    REQUIRE(InputMapper::stringToGamepadButton("B") == GamepadButton::B);
    REQUIRE(InputMapper::stringToGamepadButton("X") == GamepadButton::X);
    REQUIRE(InputMapper::stringToGamepadButton("Y") == GamepadButton::Y);
    REQUIRE(InputMapper::stringToGamepadButton("invalid") == GamepadButton::Unknown);
}

TEST_CASE("InputMapper converts enums to strings with fallback for mouse and gamepad buttons", "[unit][input_mapper]")
{
    REQUIRE(InputMapper::mouseButtonToString(MouseButton::Left) == "Left");
    REQUIRE(InputMapper::mouseButtonToString(MouseButton::Unknown) == "Unknown");

    REQUIRE(InputMapper::gamepadButtonToString(GamepadButton::A) == "A");
    REQUIRE(InputMapper::gamepadButtonToString(GamepadButton::Unknown) == "Unknown");
}

TEST_CASE("InputMapper supports round-trip conversion for mouse and gamepad buttons", "[unit][input_mapper]")
{
    const auto mouseButton = MouseButton::Right;
    REQUIRE(InputMapper::stringToMouseButton(InputMapper::mouseButtonToString(mouseButton)) == mouseButton);

    const auto gamepadButton = GamepadButton::DpadUp;
    REQUIRE(InputMapper::stringToGamepadButton(InputMapper::gamepadButtonToString(gamepadButton)) == gamepadButton);
}

TEST_CASE("InputMapper supports round-trip conversion for InputSource with mouse and gamepad buttons", "[unit][input_mapper]")
{
    const auto mouseSource = InputSource::mouse(MouseButton::Middle);
    REQUIRE(InputMapper::stringToSource(InputMapper::sourceToString(mouseSource)) == mouseSource);

    const auto gamepadSource = InputSource::gamepad(GamepadButton::Start);
    REQUIRE(InputMapper::stringToSource(InputMapper::sourceToString(gamepadSource)) == gamepadSource);
}

TEST_CASE("InputMapper throws for invalid source strings with mouse and gamepad buttons", "[unit][input_mapper]")
{
    REQUIRE_THROWS_AS(InputMapper::stringToSource("Mouse"), std::runtime_error);
    REQUIRE_THROWS_AS(InputMapper::stringToSource("Gamepad"), std::runtime_error);
    REQUIRE_THROWS_AS(InputMapper::stringToSource("Mouse.InvalidButton"), std::runtime_error);
    REQUIRE_THROWS_AS(InputMapper::stringToSource("Gamepad.InvalidButton"), std::runtime_error);
}

TEST_CASE("InputMapper converts known and unknown source strings", "[unit][input_mapper]")
{
    REQUIRE(InputMapper::stringToSource("Keyboard.A") == InputSource::keyboard(KeyCode::A));
    REQUIRE(InputMapper::stringToSource("Mouse.Left") == InputSource::mouse(MouseButton::Left));
    REQUIRE(InputMapper::stringToSource("Gamepad.A") == InputSource::gamepad(GamepadButton::A));
    REQUIRE_THROWS_AS(InputMapper::stringToSource("InvalidDevice.A"), std::runtime_error);
}

TEST_CASE("InputMapper converts InputSource to string and back", "[unit][input_mapper]")
{
    InputSource keyboardSource = InputSource::keyboard(KeyCode::B);
    std::string keyboardStr = InputMapper::sourceToString(keyboardSource);
    REQUIRE(InputMapper::stringToSource(keyboardStr) == keyboardSource);

    InputSource mouseSource = InputSource::mouse(MouseButton::Right);
    std::string mouseStr = InputMapper::sourceToString(mouseSource);
    REQUIRE(InputMapper::stringToSource(mouseStr) == mouseSource);

    InputSource gamepadSource = InputSource::gamepad(GamepadButton::X);
    std::string gamepadStr = InputMapper::sourceToString(gamepadSource);
    REQUIRE(InputMapper::stringToSource(gamepadStr) == gamepadSource);
}

TEST_CASE("InputMapper throws for invalid source strings with missing dot", "[unit][input_mapper]")
{
    REQUIRE_THROWS_AS(InputMapper::stringToSource("KeyboardA"), std::runtime_error);
    REQUIRE_THROWS_AS(InputMapper::stringToSource("MouseLeft"), std::runtime_error);
    REQUIRE_THROWS_AS(InputMapper::stringToSource("GamepadX"), std::runtime_error);
}
