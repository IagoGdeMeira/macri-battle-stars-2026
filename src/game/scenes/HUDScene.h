#ifndef hud_scene_h
#define hud_scene_h

#include "engine/include/Scene/Scene.h"

#include <memory>
#include <string>

class GameSettings;
class HUDVisibilitySystem;
class IFontFactory;
class ITextureFactory;
class Renderer;
class UIDrawer;
class UIFactory;
class UILoader;
class UIActionFactory;
class DataParser;
class SceneManager;
class EventBus;

class HUDScene : public Scene
{
public:
    struct Config : public Scene::Config
    {
        float initialRoundTime = 99.f;
        std::string layoutPath, healthBarWidgetPath;
    };

    explicit HUDScene(Config&& cfg);

    ~HUDScene() override;

    void init() override;
    void onPause() override;
    void onResume() override;
    void render() override;

    bool allowsUpdateBelow() const override { return true; }

private:
    float initialRoundTime = 99.f;
    std::string layoutPath;
    std::string healthBarWidgetPath;

    EventBus& eventBus;
    SceneManager& sceneManager;
    DataParser& parser;
    Renderer& renderer;
    GameSettings& settings;
    IFontFactory& fontFactory;
    ITextureFactory& textureFactory;

    std::unique_ptr<UIFactory> uiFactory;
    std::unique_ptr<UIDrawer> uiDrawer;
    std::unique_ptr<UILoader> uiLoader;
    std::unique_ptr<UIActionFactory> actionFactory;

    HUDVisibilitySystem* visibilitySystem = nullptr;
    Entity hudRoot;

    void registerComponents();
    void addSystems();
    void loadHUDLayout();
    void registerWidgetLoaders();
};

#endif // hud_scene_h
