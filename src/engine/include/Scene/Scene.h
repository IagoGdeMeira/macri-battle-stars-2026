#ifndef scene_h
#define scene_h

#include "SystemManager/SystemManager.h"
#include "UpdateContext/UpdateContext.h"

#include "domain/include/World/World.h"

#include <utility>

class DataParser;
class Engine;
class EventBus;
class GameSettings;
class IFontFactory;
class IPlatformFactory;
class ITextureFactory;
class Renderer;
class ResourceManager;
class SceneManager;
class TextureLoader;
class Window;

class Scene
{
public:
    struct Config
    {
        EventBus* eventBus = nullptr;
        SceneManager* sceneManager = nullptr;
        Renderer* renderer = nullptr;
        Window* window = nullptr;
        DataParser* parser = nullptr;
        ResourceManager* resourceManager = nullptr;
        TextureLoader* textureLoader = nullptr;
        GameSettings* settings = nullptr;
        Engine* engine = nullptr;
        IFontFactory* fontFactory = nullptr;
        IPlatformFactory* platformFactory = nullptr;
        ITextureFactory* textureFactory = nullptr;
    };

    Scene(EventBus& eventBus) : eventBus(eventBus) {}
    virtual ~Scene() = default;

    virtual void init() {}
    virtual void update(float deltaTime);
    virtual void render() {}

    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void onPause() {}
    virtual void onResume() {}

    virtual bool allowsUpdateBelow() const { return false; }

    World& world() { return this->localWorld; }
    const World& world() const { return this->localWorld; }

    SystemManager& systems() { return this->systemManager; }
    const SystemManager& systems() const { return this->systemManager; }

    template <typename T, typename... Args>
    T& addSystem(Args&&... args) { return this->systemManager.addSystem<T>(std::forward<Args>(args)...); }

protected:
    World localWorld; 
    SystemManager systemManager;
    EventBus& eventBus;
};

#endif // scene_h
