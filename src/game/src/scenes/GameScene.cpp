#include "../../scenes/GameScene/GameScene.h"

#include "../../include/AnimationStateSystem/AnimationStateSystem.h"
#include "../../include/AnimationSystem/AnimationSystem.h"
#include "../../include/CameraControllerSystem/CameraControllerSystem.h"
#include "../../include/CollisionDetectionSystem/CollisionDetectionSystem.h"
#include "../../include/ComboSystem/ComboSystem.h"
#include "../../include/DamageSystem/DamageSystem.h"
#include "../../include/LocalToWorldSystem/LocalToWorldSystem.h"
#include "../../include/StateSystem/StateSystem.h"
#include "../../include/TriggerGenerationSystem/TriggerGenerationSystem.h"

#include "../../../domain/components/InputBufferComponent.h"
#include "../../../domain/components/InputComponent.h"
#include "../../../domain/components/PlayerComponent.h"
#include "../../../domain/components/StateComponent.h"
#include "../../../domain/components/StateMachineComponent.h"

#include "../../../engine/include/InputBufferSystem/InputBufferSystem.h"
#include "../../../engine/include/InputSystem/InputSystem.h"
#include "../../../engine/include/MovementSystem/MovementSystem.h"

#include <utility>

GameScene::GameScene(Config&& config) :
    Scene(config.eventBus),
    input(config.input),
    triggerContext(std::move(config.triggerContext)),
    camera(config.camera),
    window(config.window),
    combos(std::move(config.combos)),
    characterLoader(config.characterLoader),
    playerSlots(std::move(config.playerSlots)),
    renderer(config.renderer)
{
    systemManager.addSystem<InputSystem>(config.eventBus, this->input);
    systemManager.addSystem<TriggerGenerationSystem>(config.eventBus, this->triggerContext);
    systemManager.addSystem<InputBufferSystem>(config.eventBus, this->input);
    systemManager.addSystem<ComboSystem>(config.eventBus, this->combos);

    systemManager.addSystem<StateSystem>(config.eventBus);

    systemManager.addSystem<MovementSystem>();
    systemManager.addSystem<LocalToWorldSystem>();

    systemManager.addSystem<AnimationStateSystem>(config.eventBus);
    systemManager.addSystem<AnimationSystem>();

    systemManager.addSystem<CollisionDetectionSystem>();
    systemManager.addSystem<DamageSystem>(config.eventBus);

    systemManager.addSystem<CameraControllerSystem>(this->camera, this->window);

    this->renderSystem = std::make_unique<RenderSystem>(
        config.eventBus, config.renderer, config.camera);
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

        components.add<PlayerComponent>(entity, PlayerComponent{ slot.playerId });
        components.add<InputComponent>(entity, InputComponent{});
        components.add<InputBufferComponent>(entity, InputBufferComponent{});

        if (!components.has<StateComponent>(entity)) continue;
        
        auto& state = components.get<StateComponent>(entity);
        state.current = StateId::Idle;
    }
}

void GameScene::render()
{
    RenderContext ctx { this->world(), this->eventBus };
    this->renderSystem->draw(ctx);
}
