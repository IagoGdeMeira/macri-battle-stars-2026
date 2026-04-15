#include "../include/GameScene/GameScene.h"

#include "../include/CameraControllerSystem/CameraControllerSystem.h"
#include "../include/ComboSystem/ComboSystem.h"
#include "../include/StateSystem/StateSystem.h"
#include "../include/TriggerGenerationSystem/TriggerGenerationSystem.h"

#include "../../domain/components/InputBufferComponent.h"
#include "../../domain/components/InputComponent.h"
#include "../../domain/components/PlayerComponent.h"
#include "../../domain/components/StateComponent.h"
#include "../../domain/components/StateMachineComponent.h"

#include "../../engine/include/InputBufferSystem/InputBufferSystem.h"
#include "../../engine/include/InputSystem/InputSystem.h"

#include <utility>

GameScene::GameScene(
    EventBus& bus,
    InputContext& input,
    TriggerContext triggerContext,
    std::vector<Combo> combos,
    StateMachine machine,
    Camera2D& camera,
    Window& window
) :
    Scene(bus),
    input(input),
    triggerContext(std::move(triggerContext)),
    machine(std::move(machine)),
    camera(camera),
    window(window)
{
    systemManager.addSystem<InputSystem>(bus, input);
    systemManager.addSystem<TriggerGenerationSystem>(bus, this->triggerContext);
    systemManager.addSystem<InputBufferSystem>(bus, input);
    systemManager.addSystem<ComboSystem>(bus, combos);
    systemManager.addSystem<StateSystem>(bus);
    systemManager.addSystem<CameraControllerSystem>(camera, window);
}

void GameScene::init()
{
    auto& components = this->world().components();

    components.registerComponent<PlayerComponent>();
    components.registerComponent<InputComponent>();
    components.registerComponent<InputBufferComponent>();
    components.registerComponent<StateComponent>();
    components.registerComponent<StateMachineComponent>();

    const auto entity = this->world().entities().create();

    PlayerId playerId = 0;

    if (!this->input.bindings.empty())
    { playerId = this->input.bindings.begin()->first; }

    components.add<PlayerComponent>(entity, PlayerComponent{ playerId });
    components.add<InputComponent>(entity, InputComponent{});
    components.add<InputBufferComponent>(entity, InputBufferComponent{});
    components.add<StateComponent>(entity, StateComponent{});
    components.add<StateMachineComponent>(
        entity, StateMachineComponent{ std::move(this->machine) });
}
