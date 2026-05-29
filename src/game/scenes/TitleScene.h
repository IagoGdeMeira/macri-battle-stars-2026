#ifndef title_scene_h
#define title_scene_h

#include "../../engine/include/DataParser/DataParser.h"
#include "../../engine/include/Engine/Engine.h"
#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/IFontFactory/IFontFactory.h"
#include "../../engine/include/ITextureFactory/ITextureFactory.h"
#include "../../engine/include/Renderer/Renderer.h"
#include "../../engine/include/ResourceManager/ResourceManager.h"
#include "../../engine/include/Scene/Scene.h"
#include "../../engine/include/SceneManager/SceneManager.h"

#include <memory>

class MenuLoader;
class UIDrawer;
class UIFactory;
class UIInputSystem;
class UILayoutSystem;

struct InputContext;

class TitleScene : public Scene
{
public:
    struct Config : public Scene::Config {};

    explicit TitleScene(Config&& cfg) :
        Scene(*cfg.eventBus),
        eventBus(*cfg.eventBus),
        sceneManager(*cfg.sceneManager),
        renderer(*cfg.renderer),
        parser(*cfg.parser),
        resourceManager(*cfg.resourceManager),
        fontFactory(*cfg.fontFactory),
        textureFactory(*cfg.textureFactory),
        engine(*cfg.engine) {}

    ~TitleScene() override = default;

    void init() override;
    void onEnter() override;
    void onExit() override;

    Scene::UpdatePolicy getUpdatePolicy() const override { return Scene::UpdatePolicy::WhenTop; }

private:
    // Dependências injetadas
    EventBus& eventBus;
    SceneManager& sceneManager;
    Renderer& renderer;
    DataParser& parser;
    ResourceManager& resourceManager;
    IFontFactory& fontFactory;
    ITextureFactory& textureFactory;
    Engine& engine;

    // Sistemas e fábricas de UI
    std::unique_ptr<UIFactory> uiFactory;
    std::unique_ptr<MenuLoader> menuLoader;
    std::unique_ptr<UIDrawer> uiDrawer;

    // Input
    std::unique_ptr<InputContext> inputContext;

    void loadInputContext();
    void setupInputAdapters();
    void loadMenu();
    void onUIAction(const UIActionEvent& e);
};

#endif // title_scene_h
