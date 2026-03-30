#include "../../src/game/include/InputBindingLoader/InputBindingLoader.h"

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

TEST_CASE("InputBindingLoader converts known keys and actions",
    "[unit][input_binding_loader]"
) {
    REQUIRE(InputBindingLoader::stringToKeyCode("A") == KeyCode::A);
    REQUIRE(InputBindingLoader::stringToKeyCode("Escape") == KeyCode::Escape);

    REQUIRE(InputBindingLoader::stringToInputAction("MoveUp") == InputAction::MoveUp);
    REQUIRE(InputBindingLoader::stringToInputAction("Attack") == InputAction::Attack);
}

TEST_CASE("InputBindingLoader returns fallback for unknown key/action",
    "[unit][input_binding_loader]"
) {
    REQUIRE(InputBindingLoader::stringToKeyCode("NotAKey") == KeyCode::Unknown);
    REQUIRE(InputBindingLoader::stringToInputAction("Dance") == InputAction::None);
}

TEST_CASE("InputBindingLoader loads a valid json file without throwing",
    "[unit][input_binding_loader]"
) {
    const auto tmp = std::filesystem::temp_directory_path() / "mbs_input_binding_loader_valid.json";

    {
        std::ofstream out(tmp.string());
        out << R"({
            "players": [
                {
                    "id": 1,
                    "bindings": {
                        "MoveUp": "W",
                        "Attack": "Space"
                    }
                }
            ]}
        )";
    }

    REQUIRE_NOTHROW(InputBindingLoader::load(tmp.string()));

    std::filesystem::remove(tmp);
}
