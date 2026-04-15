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

GameScene::GameScene(Config config) :
    Scene(config.eventBus),
    input(config.input),
    triggerContext(std::move(config.triggerContext)),
    camera(config.camera),
    window(config.window),
    playerSlots(std::move(config.playerSlots)),
    characterLoader(config.characterLoader),
    combos(std::move(config.combos))
{
    this->systemManager.addSystem<InputSystem>(this->eventBus, this->input);
    this->systemManager.addSystem<TriggerGenerationSystem>(this->eventBus, this->triggerContext);
    this->systemManager.addSystem<InputBufferSystem>(this->eventBus, this->input);
    this->systemManager.addSystem<ComboSystem>(this->eventBus, combos);
    this->systemManager.addSystem<StateSystem>(this->eventBus);
    this->systemManager.addSystem<CameraControllerSystem>(this->camera, this->window);
}

void GameScene::init()
{
    auto& world = this->world();
    auto& components = world.components();

    components.registerComponent<PlayerComponent>();
    components.registerComponent<InputComponent>();
    components.registerComponent<InputBufferComponent>();

    for (const auto& slot : playerSlots)
    {
        Entity entity = characterLoader.create(world, slot.characterDefPath);

        components.add<PlayerComponent>(entity, PlayerComponent{ slot.PlayerId });
        components.add<InputComponent>(entity, InputComponent{});
        components.add<InputBufferComponent>(entity, InputBufferComponent{});

        if (components.has<StateComponent>(entity))
        { components.get<StateComponent>(entity).current = StateId::Idle; }
    }
}
