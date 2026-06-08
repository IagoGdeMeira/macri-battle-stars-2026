#include "../../../src/game/ui_actions/NavigateToSelectionAction.h"

#include "../../../src/domain/include/Color/Color.h"
#include "../../../src/domain/include/Geometry/Geometry.h"

#include "../../../src/engine/include/DataNode/DataNode.h"
#include "../../../src/engine/include/DataParser/DataParser.h"
#include "../../../src/engine/include/Engine/Engine.h"
#include "../../../src/engine/include/EventBus/EventBus.h"
#include "../../../src/engine/include/GameSettings/GameSettings.h"
#include "../../../src/engine/include/IFontFactory/IFontFactory.h"
#include "../../../src/engine/include/IPlatformFactory/IPlatformFactory.h"
#include "../../../src/engine/include/ITextureFactory/ITextureFactory.h"
#include "../../../src/engine/include/Renderer/Renderer.h"
#include "../../../src/engine/include/ResourceManager/ResourceManager.h"
#include "../../../src/engine/include/Scene/Scene.h"
#include "../../../src/engine/include/SceneFactory/SceneFactory.h"
#include "../../../src/engine/include/SceneManager/SceneManager.h"
#include "../../../src/engine/include/TextureLoader/TextureLoader.h"
#include "../../../src/engine/include/ThreadPool/ThreadPool.h"
#include "../../../src/engine/include/Viewport/Viewport.h"
#include "../../../src/engine/include/Window/Window.h"

#include "../../../src/game/scenes/SelectionScene.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>

class NavigateToSelectionActionFixture
{
public:
    class StubWindow : public Window
    {
    public:
        void create(int,int,const char*) override {}
        void setResolution(int,int) override {}
        void setFullscreen(bool) override {}
        void getSize(int& w,int& h) override { w=800; h=600; }
    };

    class StubRenderer : public Renderer
    {
    public:
        void clear() override {}
        void present() override {}
        void drawTexture(const DrawTextureCommand&) override {}
        void drawFont(const DrawFontCommand&) override {}
        void drawRectangle(const DrawRectangleCommand&) override {}
        void drawCircle(const DrawCircleCommand&) override {}
        void setViewport(const Viewport&) override {}
    };

    class StubDataParser : public DataParser
    {
    public:
        std::unique_ptr<DataNode> parse(const std::string&) const override { return nullptr; }
    };

    class StubResourceManager : public ResourceManager
    {
    public:
        StubResourceManager() : ResourceManager(this->pool) {}
    private:
        ThreadPool pool{1};
    };

    class StubTextureFactory : public ITextureFactory
    {
    public:
        std::shared_ptr<Texture> createTexture(const std::string&) override { return nullptr; }
    };

    class StubFontFactory : public IFontFactory
    {
    public:
        std::shared_ptr<Font> createFont(const std::string&) override { return nullptr; }
    };

    class StubPlatformFactory : public IPlatformFactory
    {
    public:
        std::unique_ptr<Window> createWindow(int, int, const std::string&) override { return nullptr; }
        std::unique_ptr<Renderer> createRenderer(Window&) override { return nullptr; }
        InputAdapterPtr createKeyboardAdapter(EventBus&, const InputContext&) override { return nullptr; }
        InputAdapterPtr createMouseAdapter(EventBus&) override { return nullptr; }
        std::vector<InputAdapterPtr> createGamepadAdapters(EventBus&, uint32_t) override { return {}; }
        InputAdapterPtr createSystemAdapter(EventBus&) override { return nullptr; }
        std::unique_ptr<IPlatformEventProvider> createEventProvider() override { return nullptr; }
        std::unique_ptr<ITextureFactory> createTextureFactory(Renderer&) override { return nullptr; }
        std::unique_ptr<ISoundFactory> createSoundFactory() override { return nullptr; }
        std::unique_ptr<IMusicFactory> createMusicFactory() override { return nullptr; }
        std::unique_ptr<IFontFactory> createFontFactory() override { return nullptr; }
    };

    class StubTextureLoader : public TextureLoader
    {
    public:
        StubTextureLoader(ITextureFactory& factory) : TextureLoader(factory) {}
    };

    NavigateToSelectionActionFixture()
    {
        this->engine = std::make_unique<Engine>(this->window, this->settings);
        this->engine->setRenderer(this->renderer);

        this->textureFactory = std::make_unique<StubTextureFactory>();
        this->fontFactory = std::make_unique<StubFontFactory>();
        this->textureLoader = std::make_unique<StubTextureLoader>(*this->textureFactory);

        this->factory = std::make_unique<SceneFactory>(SceneFactory::Config{
            .window             = this->window,
            .parser             = this->dataParser,
            .resourceManager    = this->resourceManager,
            .textureLoader      = *this->textureLoader,
            .renderer           = this->renderer,
            .eventBus           = this->engine->events(),
            .settings           = this->settings,
            .engine             = *this->engine,
            .fontFactory        = *this->fontFactory,
            .textureFactory     = *this->textureFactory
        });

        this->sceneManager = std::make_unique<SceneManager>(*this->factory, *this->engine);
        this->engine->setSceneManager(*this->sceneManager);
    }

    SceneManager& scenes() { return this->engine->scenes(); }

    StubWindow window;
    StubRenderer renderer;
    GameSettings settings;
    std::unique_ptr<Engine> engine;
    std::unique_ptr<SceneFactory> factory;
    std::unique_ptr<SceneManager> sceneManager;
    StubDataParser dataParser;
    StubResourceManager resourceManager;
    std::unique_ptr<StubTextureLoader> textureLoader;
    std::unique_ptr<StubTextureFactory> textureFactory;
    std::unique_ptr<StubFontFactory> fontFactory;
    StubPlatformFactory platformFactory;
};

TEST_CASE_METHOD(NavigateToSelectionActionFixture, "NavigateToSelectionAction changes to Selection scene",
    "[unit][navigate_to_selection_action]"
) {
    NavigateToSelectionAction action(scenes());

    action.execute();

    REQUIRE(scenes().currentScene() != nullptr);
    REQUIRE(dynamic_cast<SelectionScene*>(scenes().currentScene()) != nullptr);
}
