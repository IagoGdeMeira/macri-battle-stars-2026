#define SDL_MAIN_HANDLED

#include "../engine/include/Engine/Engine.h"

#include "../platform/include/SDLWindow/SDLWindow.h"

int main()
{
    SDLWindow window;
    window.create(800, 600, "Macri Battle Stars");

    Engine engine(window);
    engine.run();

    return 0;
}
