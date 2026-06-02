#ifndef scene_factory_h
#define scene_factory_h

#include "../DataParser/DataParser.h"
#include "../EventBus/EventBus.h"
#include "../GameSettings/GameSettings.h"
#include "../IFontFactory/IFontFactory.h"
#include "../ITextureFactory/ITextureFactory.h"
#include "../Renderer/Renderer.h"
#include "../ResourceManager/ResourceManager.h"
#include "../Scene/Scene.h"
#include "../TextureLoader/TextureLoader.h"
#include "../Window/Window.h"

#include <memory>

class Engine;
class SceneManager;

class SceneFactory
{
public:
    struct Config
    {
        Window& window;
        DataParser& parser;
        ResourceManager& resourceManager;
        TextureLoader& textureLoader;
        Renderer& renderer;
        EventBus& eventBus;
        SceneManager& sceneManager;
        GameSettings& settings;
        Engine& engine;
        IFontFactory& fontFactory;
        ITextureFactory& textureFactory;
    };

    explicit SceneFactory(Config&& config) : 
        window(config.window),
        parser(config.parser),
        resourceManager(config.resourceManager),
        textureLoader(config.textureLoader),
        renderer(config.renderer),
        eventBus(config.eventBus),
        sceneManager(config.sceneManager),
        settings(config.settings),
        engine(config.engine),
        fontFactory(config.fontFactory),
        textureFactory(config.textureFactory) {}

    template <typename SceneType>
    std::unique_ptr<Scene> createScene(typename SceneType::Config cfg);

private:
    Window& window;
    DataParser& parser;
    ResourceManager& resourceManager;
    TextureLoader& textureLoader;
    Renderer& renderer;
    EventBus& eventBus;
    SceneManager& sceneManager;
    GameSettings& settings;
    Engine& engine;
    IFontFactory& fontFactory;
    ITextureFactory& textureFactory;
};

#include "SceneFactory.inl"

#endif // scene_factory_h
