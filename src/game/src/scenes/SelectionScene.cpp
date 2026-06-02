#include "../scenes/SelectionScene.h"

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
{ /* TODO */}

void SelectionScene::update(float)
{ /* TODO */ }
