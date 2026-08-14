#ifndef hud_scene_h
#define hud_scene_h

#include "engine/include/Scene/Scene.h"

#include <memory>

class Renderer;
class GameSettings;
class IFontFactory;
class ITextureFactory;
class UIFactory;
class UIDrawer;
class HUDVisibilitySystem;

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
    float initialRoundTime = 99.f;

    Renderer& renderer;
    GameSettings& settings;
    IFontFactory& fontFactory;
    ITextureFactory& textureFactory;

    std::unique_ptr<UIFactory> uiFactory;
    std::unique_ptr<UIDrawer> uiDrawer;

    HUDVisibilitySystem* visibilitySystem = nullptr;

    void registerComponents();
    void addSystems();

    struct HealthBarParams { uint32_t playerId; std::string playerName; int maxHealth = 0, currentHealth = 0; };
    void createHealthBar(const HealthBarParams& params);
    
    void createTimer(float initialTime);
};

#endif // hud_scene_h
