#include "../scenes/TitleScene.h"

#include "../../engine/include/InputBindingLoader/InputBindingLoader.h"
#include "../../engine/include/InputContext/InputContext.h"
#include "../../engine/include/InputManager/InputManager.h"
#include "../../engine/include/TextureLoader/TextureLoader.h"
#include "../../engine/include/RenderContext/RenderContext.h"

#include "../../game/include/MenuLoader/MenuLoader.h"
#include "../../game/include/UIActionFactory/UIActionFactory.h"
#include "../../game/include/UIDrawer/UIDrawer.h"
#include "../../game/include/UIFactory/UIFactory.h"
#include "../../game/include/UIInputSystem/UIInputSystem.h"
#include "../../game/include/UILayoutSystem/UILayoutSystem.h"

void TitleScene::init()
{
    this->loadInputContext();
    this->setupInputAdapters();

    this->uiFactory = std::make_unique<UIFactory>(this->world(), this->fontFactory, this->textureFactory);

    UIActionFactory::Config actionCfg{ this->eventBus, this->sceneManager };
    auto actionFactory = std::make_shared<UIActionFactory>(std::move(actionCfg));

    this->menuLoader = std::make_unique<MenuLoader>(this->parser, *actionFactory);

    this->loadMenu();

    this->addSystem<UILayoutSystem>();
    this->addSystem<UIInputSystem>(this->eventBus);

    this->uiDrawer = std::make_unique<UIDrawer>(this->eventBus, this->renderer, this->settings);
}

void TitleScene::loadInputContext()
{
    InputBindingLoader loader(this->parser);
    this->inputContext = std::make_unique<InputContext>(loader.load("assets/inputs/input_bindings.json"));
}

void TitleScene::setupInputAdapters()
{
    auto& inputManager = this->engine.input();

    inputManager.addAdapter(this->platformFactory.createKeyboardAdapter(this->eventBus, *this->inputContext));
    inputManager.addAdapter(this->platformFactory.createMouseAdapter(this->eventBus));

    auto gamepadAdapters = this->platformFactory.createGamepadAdapters(this->eventBus, 1);
    for (auto& adapter : gamepadAdapters) inputManager.addAdapter(std::move(adapter));

    inputManager.addAdapter(this->platformFactory.createSystemAdapter(this->eventBus));
}

void TitleScene::loadMenu()
{
    auto menuData = this->menuLoader->load("assets/menus/title.json");
    for (auto& elem : menuData.elements) this->uiFactory->createFromElement(*elem);
}

void TitleScene::render()
{
    RenderContext ctx{ this->world(), this->eventBus };
    this->uiDrawer->draw(ctx);
}
