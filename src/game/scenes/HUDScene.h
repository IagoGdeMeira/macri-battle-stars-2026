#ifndef hud_scene_h
#define hud_scene_h

#include "domain/components/FlexContainer.h"

#include "engine/include/Scene/Scene.h"

#include <memory>

class GameSettings;
class HUDVisibilitySystem;
class IFontFactory;
class ITextureFactory;
class Renderer;
class UIDrawer;
class UIFactory;

class HUDScene : public Scene
{
public:
    struct Config : public Scene::Config { float initialRoundTime = 99.f; };

    explicit HUDScene(Config&& cfg);

    ~HUDScene() override;

    void init() override;
    void onPause() override;
    void onResume() override;
    void render() override;

    bool allowsUpdateBelow() const override { return true; }

private:
    using Direction = FlexContainer::FlexDirection;
    using Align = FlexContainer::AlignItems;
    using Justify = FlexContainer::JustifyContent;

    float initialRoundTime = 99.f;

    Renderer& renderer;
    GameSettings& settings;
    IFontFactory& fontFactory;
    ITextureFactory& textureFactory;

    std::unique_ptr<UIFactory> uiFactory;
    std::unique_ptr<UIDrawer> uiDrawer;

    HUDVisibilitySystem* visibilitySystem = nullptr;
    Entity hudRoot;

    void registerComponents();
    void addSystems();

    struct HealthBarParams { uint32_t playerId; std::string playerName; int maxHealth = 0, currentHealth = 0; };
    void createHealthBar(const HealthBarParams& params);
    void createTimer(float initialTime);
    void createRoot();
};

#endif // hud_scene_h
