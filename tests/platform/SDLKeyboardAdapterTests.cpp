#include "../../src/platform/include/SDLKeyboardAdapter/SDLKeyboardAdapter.h"

#include "../../src/engine/events/InputEvent.h"
#include "../../src/engine/events/QuitEvent.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/InputBinding/InputBinding.h"
#include "../../src/engine/include/InputContext/InputContext.h"
#include "../../src/engine/include/KeyCode/KeyCode.h"

#include <catch2/catch_test_macros.hpp>
#include <SDL.h>
#include <vector>

TEST_CASE("SDLKeyboardAdapter emits quit and digital input events",
    "[integration][sdl_keyboard_adapter]"
) {
    REQUIRE(SDL_Init(SDL_INIT_EVENTS) == 0);

    InputContext inputContext;
    InputBinding binding;
    binding.keyMap[KeyCode::A] = InputAction::MoveLeft;
    inputContext.bindings[0] = binding;

    EventBus bus;
    SDLKeyboardAdapter adapter(bus, inputContext);

    std::vector<DigitalInputEvent> receivedInputs;
    int quitCount = 0;

    bus.subscribe<DigitalInputEvent>([&](const DigitalInputEvent& event)
    { receivedInputs.push_back(event); });

    bus.subscribe<QuitEvent>([&](const QuitEvent&)
    { quitCount++; });

    SDL_Event keyDown {};
    keyDown.type = SDL_KEYDOWN;
    keyDown.key.keysym.sym = SDLK_a;

    SDL_Event keyUp {};
    keyUp.type = SDL_KEYUP;
    keyUp.key.keysym.sym = SDLK_a;

    SDL_Event quit {};
    quit.type = SDL_QUIT;

    REQUIRE(SDL_PushEvent(&keyDown) == 1);
    REQUIRE(SDL_PushEvent(&keyUp) == 1);
    REQUIRE(SDL_PushEvent(&quit) == 1);

    adapter.poll();

    REQUIRE(receivedInputs.size() == 2);
    REQUIRE(receivedInputs[0].source == InputSource::keyboard(KeyCode::A));
    REQUIRE(receivedInputs[0].playerId == 0);
    REQUIRE(receivedInputs[0].pressed == true);
    REQUIRE(receivedInputs[1].source == InputSource::keyboard(KeyCode::A));
    REQUIRE(receivedInputs[1].playerId == 0);
    REQUIRE(receivedInputs[1].pressed == false);
    REQUIRE(quitCount == 1);

    SDL_Quit();
}
