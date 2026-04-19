#include "../../scenes/TitleScene/TitleScene.h"

TitleScene::TitleScene(Config config) :
    Scene(config.eventBus),
    sceneManager(config.sceneManager),
    roster(config.roster),
    input(config.input),
    renderer(config.renderer)
{
    // TODO: Init UI and other title screen elements
}

void TitleScene::update(float)
{
    // TODO: Update title screen elements
}
