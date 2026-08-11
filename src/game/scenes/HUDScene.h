#ifndef hud_scene_h
#define hud_scene_h

#include "engine/include/Scene/Scene.h"

#include <memory>

class UIFactory;
class UIDrawer;
class Renderer;
class GameSettings;
class IFontFactory;
class ITextureFactory;

class HUDScene : public Scene
{
public:
    struct Config : public Scene::Config {};
    
    explicit HUDScene(Config&& cfg);
    ~HUDScene() override = default;

    void init() override;
    void onPause() override;
    void onResume() override;
    void render() override;

    bool allowsUpdateBelow() const override { return true; }

private:
    Renderer& renderer;
    GameSettings& settings;
    IFontFactory& fontFactory;
    ITextureFactory& textureFactory;

    std::unique_ptr<UIFactory> uiFactory;
    std::unique_ptr<UIDrawer> uiDrawer;

    void registerComponents();
    void addSystems();
    void createHealthBar(uint32_t playerId, int maxHealth, int currentHealth);
};

#endif // hud_scene_h
