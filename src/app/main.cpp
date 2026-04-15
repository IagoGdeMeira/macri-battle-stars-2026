#define SDL_MAIN_HANDLED

#include "../engine/include/Engine/Engine.h"
#include "../engine/include/InputBindingLoader/InputBindingLoader.h"
#include "../engine/include/Window/Window.h"

#include "../game/include/Camera2D/Camera2D.h"
#include "../game/include/ComboLoader/ComboLoader.h"
#include "../game/include/GameScene/GameScene.h"
#include "../game/include/StateMachineLoader/StateMachineLoader.h"
#include "../game/include/TriggerBindingLoader/TriggerBindingLoader.h"

#include "../platform/include/JsonParser/JsonParser.h"
#include "../platform/include/SDLInputAdapter/SDLInputAdapter.h"
#include "../platform/include/SDLWindow/SDLWindow.h"

#include <utility>

int main()
{
    SDLWindow window;
    window.create(800, 600, "Macri Battle Stars");

    JsonParser parser;
    InputBindingLoader loader(parser);
    auto bindings = loader.load("assets/inputs/input_bindings.json");

    TriggerBindingLoader triggerBindingLoader(parser);
    auto triggerContext = triggerBindingLoader.load("assets/fsm/trigger_bindings.json");

    ComboLoader comboLoader(parser);
    auto combos = comboLoader.load("assets/combos/combos.json");

    StateMachineLoader stateMachineLoader(parser);
    auto machine = stateMachineLoader.load("assets/fsm/state_machine.json");

    Engine engine(window);

    SDLInputAdapter input(engine.events());
    engine.setInputAdapter(input);

    Camera2D camera;

    engine.scenes().changeScene<GameScene>(
        GameScene::Config{
            engine.events(),
            bindings,
            std::move(triggerContext),
            combos,
            std::move(machine),
            camera,
            window
        }
    );
    engine.run();

    return 0;
}
