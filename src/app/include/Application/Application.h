#ifndef application_h
#define application_h

#include "../ISystemInitializer/ISystemInitializer.h"

#include "../../domain/include/Geometry/Geometry.h"

#include "../../engine/include/DataParser/DataParser.h"
#include "../../engine/include/Engine/Engine.h"
#include "../../engine/include/GameSettings/GameSettings.h"
#include "../../engine/include/IPlatformFactory/IPlatformFactory.h"
#include "../../engine/include/SceneFactory/SceneFactory.h"

#include <memory>
#include <string>

class Renderer;
class ResourceManager;
class TextureLoader;
class ThreadPool;
class Window;

class Application
{
public:
    Application() = default;
    ~Application() = default;
    
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    Application& setWindowTitle(const std::string& title);
    Application& setWindowSize(int width, int height);

    int run();

private:
    std::string windowTitle = "Macri Battle Stars";
    Dimension2D windowSize {800.f, 600.f};
    GameSettings gameSettings;

    std::unique_ptr<ISystemInitializer> initializer;
    std::unique_ptr<IPlatformFactory> platformFactory;
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<SceneFactory> sceneFactory;
    std::unique_ptr<Engine> engine;
    std::unique_ptr<DataParser> parser;
    std::unique_ptr<ThreadPool> threadPool;
    std::unique_ptr<ResourceManager> resourceManager;
    std::unique_ptr<TextureLoader> textureLoader;

    void initSystems();
    void initLoaders();
    void setupInitialScene();
};

#endif // application_h
