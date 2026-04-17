#include "../../scenes/GameScene/GameScene.h"

#include "../../include/CameraControllerSystem/CameraControllerSystem.h"
#include "../../include/ComboSystem/ComboSystem.h"
#include "../../include/StateSystem/StateSystem.h"
#include "../../include/TriggerGenerationSystem/TriggerGenerationSystem.h"

#include "../../../domain/components/InputBufferComponent.h"
#include "../../../domain/components/InputComponent.h"
#include "../../../domain/components/PlayerComponent.h"
#include "../../../domain/components/StateComponent.h"
#include "../../../domain/components/StateMachineComponent.h"

#include "../../../engine/include/InputBufferSystem/InputBufferSystem.h"
#include "../../../engine/include/InputSystem/InputSystem.h"

#include <utility>

GameScene::GameScene(Config&& config) :
    Scene(config.eventBus),
    input(config.input),
    triggerContext(std::move(config.triggerContext)),
    machine(),
    camera(config.camera),
    window(config.window),
    combos(std::move(config.combos)),
    characterLoader(config.characterLoader),
    playerSlots(std::move(config.playerSlots))
{
    systemManager.addSystem<InputSystem>(config.eventBus, this->input);
    systemManager.addSystem<TriggerGenerationSystem>(config.eventBus, this->triggerContext);
    systemManager.addSystem<InputBufferSystem>(config.eventBus, this->input);
    systemManager.addSystem<ComboSystem>(config.eventBus, this->combos);
    systemManager.addSystem<StateSystem>(config.eventBus);
    systemManager.addSystem<CameraControllerSystem>(this->camera, this->window);
}

void GameScene::init()
{
    auto& world = this->world();
    auto& components = this->world().components();

    components.registerComponent<PlayerComponent>();
    components.registerComponent<InputComponent>();
    components.registerComponent<InputBufferComponent>();
    components.registerComponent<StateComponent>();
    components.registerComponent<StateMachineComponent>();

    for (const auto& slot : this->playerSlots)
    {
        Entity entity = this->characterLoader.create(world, slot.characterDefPath);

        components.add<PlayerComponent>(entity, PlayerComponent{ slot.PlayerId });
        components.add<InputComponent>(entity, InputComponent{});
        components.add<InputBufferComponent>(entity, InputBufferComponent{});

        if (!components.has<StateComponent>(entity)) continue;
        
        auto& state = components.get<StateComponent>(entity);
        state.current = StateId::Idle;
    }
}
