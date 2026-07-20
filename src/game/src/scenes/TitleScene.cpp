#include "TitleScene.h"

#include "MenuLoader/MenuLoader.h"
#include "UIActionFactory/UIActionFactory.h"
#include "UIDrawer/UIDrawer.h"
#include "UIFactory/UIFactory.h"
#include "UIInputSystem/UIInputSystem.h"
#include "UILayoutSystem/UILayoutSystem.h"

#include "engine/include/InputBindingLoader/InputBindingLoader.h"
#include "engine/include/InputManager/InputManager.h"
#include "engine/include/TextureLoader/TextureLoader.h"
#include "engine/value_objects/GameConstants/GameConstants.h"
#include "engine/value_objects/InputContext/InputContext.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

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
{ this->inputContext = std::make_unique<InputContext>(InputBindingLoader(this->parser).load(GameConstants::DEFAULT_INPUT_BINDINGS_PATH)); }

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
    auto menuData = this->menuLoader->load(GameConstants::DEFAULT_TITLE_MENU_PATH);
    for (auto& elem : menuData.elements) this->uiFactory->createFromElement(*elem);
}

void TitleScene::render()
{
    RenderContext ctx{ this->world(), this->eventBus };
    this->uiDrawer->draw(ctx);
}
