#include "../../scenes/SelectionScene/SelectionScene.h"

SelectionScene::SelectionScene(Config config) :
    Scene(config.eventBus),
    sceneManager(config.sceneManager),
    roster(config.roster),
    input(config.input),
    triggerContext(std::move(config.triggerContext)),
    combos(std::move(config.combos)),
    camera(config.camera),
    window(config.window),
    characterLoader(config.characterLoader),
    renderer(config.renderer)
{
    // TODO: Init selection screen elements
    // For example, create UI entities for character portraits, selection cursors, etc.
}

void SelectionScene::update(float)
{
    // TODO: Handle input for character selection
    // For example, check for input actions to move selection cursors, confirm selections, etc.
}
