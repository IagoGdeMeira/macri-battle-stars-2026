#include "../include/GameScene/GameScene.h"

#include "../include/ComboSystem/ComboSystem.h"
#include "../include/StateSystem/StateSystem.h"

#include "../../domain/components/InputBufferComponent.h"
#include "../../domain/components/InputComponent.h"
#include "../../domain/components/PlayerComponent.h"
#include "../../domain/components/StateComponent.h"

#include "../../engine/include/InputBufferSystem/InputBufferSystem.h"
#include "../../engine/include/InputSystem/InputSystem.h"

#include <utility>

GameScene::GameScene(
    EventBus& bus,
    InputContext& input,
    std::vector<Combo> combos,
    StateMachine machine
) : Scene(bus), input(input), machine(std::move(machine))
{
    systemManager.addSystem<InputSystem>(bus, input);
    systemManager.addSystem<InputBufferSystem>(bus, input);
    systemManager.addSystem<ComboSystem>(bus, combos);
    systemManager.addSystem<StateSystem>(bus, this->machine);
}

void GameScene::init()
{
    auto& components = this->world().components();

    components.registerComponent<PlayerComponent>();
    components.registerComponent<InputComponent>();
    components.registerComponent<InputBufferComponent>();
    components.registerComponent<StateComponent>();

    const auto entity = this->world().entities().create();

    PlayerId playerId = 0;

    if (!this->input.bindings.empty())
    { playerId = this->input.bindings.begin()->first; }

    components.add<PlayerComponent>(entity, PlayerComponent{ playerId });
    components.add<InputComponent>(entity, InputComponent{});
    components.add<InputBufferComponent>(entity, InputBufferComponent{});
    components.add<StateComponent>(entity, StateComponent{});
}
