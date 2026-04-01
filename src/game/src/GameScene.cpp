#include "../include/GameScene/GameScene.h"

#include "../include/ComboSystem/ComboSystem.h"

#include "../../engine/include/InputSystem/InputSystem.h"

GameScene::GameScene(EventBus& bus, InputContext& input, std::vector<Combo> combos)
    : Scene(bus)
{
    systemManager.addSystem<InputSystem>(bus, input);
    systemManager.addSystem<ComboSystem>(bus, combos);
}
