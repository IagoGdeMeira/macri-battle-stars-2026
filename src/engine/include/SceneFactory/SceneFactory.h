#ifndef scene_factory_h
#define scene_factory_h

#include <memory>

class DataParser;
class Engine;
class IFontFactory;
class ITextureFactory;
class ResourceManager;
class TextureLoader;
class Window;

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
