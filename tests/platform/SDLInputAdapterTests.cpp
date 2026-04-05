#include "../../src/platform/include/SDLInputAdapter/SDLInputAdapter.h"

#include "../../src/engine/events/KeyEvent.h"
#include "../../src/engine/events/QuitEvent.h"
#include "../../src/engine/include/EventBus/EventBus.h"

#include <catch2/catch_test_macros.hpp>
#include <SDL.h>
#include <vector>

TEST_CASE("SDLInputAdapter emits quit and key events",
    "[integration][sdl_input_adapter]"
) {
    REQUIRE(SDL_Init(SDL_INIT_EVENTS) == 0);

    EventBus bus;
    SDLInputAdapter adapter(bus);

    std::vector<KeyEvent> receivedKeys;
    int quitCount = 0;

    bus.subscribe<KeyEvent>([&](const KeyEvent& event)
    { receivedKeys.push_back(event); });

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

    REQUIRE(receivedKeys.size() == 2);
    REQUIRE(receivedKeys[0].key == KeyCode::A);
    REQUIRE(receivedKeys[0].pressed == true);
    REQUIRE(receivedKeys[1].key == KeyCode::A);
    REQUIRE(receivedKeys[1].pressed == false);
    REQUIRE(quitCount == 1);

    SDL_Quit();
}
