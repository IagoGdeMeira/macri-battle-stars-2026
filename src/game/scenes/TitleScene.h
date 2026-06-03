#ifndef title_scene_h
#define title_scene_h

#include "../include/MenuLoader/MenuLoader.h"
#include "../include/UIDrawer/UIDrawer.h"
#include "../include/UIFactory/UIFactory.h"

#include "../../engine/include/DataParser/DataParser.h"
#include "../../engine/include/Engine/Engine.h"
#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/GameSettings/GameSettings.h"
#include "../../engine/include/IFontFactory/IFontFactory.h"
#include "../../engine/include/IPlatformFactory/IPlatformFactory.h"
#include "../../engine/include/ITextureFactory/ITextureFactory.h"
#include "../../engine/include/Renderer/Renderer.h"
#include "../../engine/include/ResourceManager/ResourceManager.h"
#include "../../engine/include/Scene/Scene.h"

#include <memory>

struct InputContext;

class TitleScene : public Scene
{
public:
    struct Config : public Scene::Config { IPlatformFactory* platformFactory = nullptr; };

    explicit TitleScene(Config&& cfg) :
        Scene(*cfg.eventBus),
        eventBus(*cfg.eventBus),
        sceneManager(*cfg.sceneManager),
        renderer(*cfg.renderer),
        parser(*cfg.parser),
        resourceManager(*cfg.resourceManager),
        fontFactory(*cfg.fontFactory),
        textureFactory(*cfg.textureFactory),
        platformFactory(*cfg.platformFactory),
        engine(*cfg.engine),
        settings(*cfg.settings) {}
    ~TitleScene() override = default;

    void init() override;
    void render() override;
    void onEnter() override {}
    void onExit() override {}

    Scene::UpdatePolicy getUpdatePolicy() const override { return Scene::UpdatePolicy::WhenTop; }

private:
    EventBus& eventBus;
    SceneManager& sceneManager;
    Renderer& renderer;
    DataParser& parser;
    ResourceManager& resourceManager;
    IFontFactory& fontFactory;
    ITextureFactory& textureFactory;
    IPlatformFactory& platformFactory;
    Engine& engine;
    GameSettings& settings;

    std::unique_ptr<UIFactory> uiFactory;
    std::unique_ptr<MenuLoader> menuLoader;
    std::unique_ptr<UIDrawer> uiDrawer;

    std::unique_ptr<InputContext> inputContext;

    void loadInputContext();
    void setupInputAdapters();
    void loadMenu();
};

#endif // title_scene_h
