#define SDL_MAIN_HANDLED

#include "../engine/include/Engine/Engine.h"

#include "../game/include/InputBindingLoader/InputBindingLoader.h"

#include "../platform/include/JsonParser/JsonParser.h"
#include "../platform/include/SDLInputAdapter/SDLInputAdapter.h"
#include "../platform/include/SDLWindow/SDLWindow.h"

int main()
{
    SDLWindow window;
    window.create(800, 600, "Macri Battle Stars");

    JsonParser parser;
    InputBindingLoader loader(parser);
    auto bindings = loader.load("assets/inputs/input_bindings.json");

    Engine engine(window);

    SDLInputAdapter input(engine.events());
    engine.setInputAdapter(input);

    engine.run();

    return 0;
}
