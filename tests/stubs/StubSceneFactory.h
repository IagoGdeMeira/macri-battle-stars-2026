#ifndef STUB_SCENE_FACTORY_H
#define STUB_SCENE_FACTORY_H

#include "StubDataParser.h"
#include "StubEngine.h"
#include "StubFontFactory.h"
#include "StubRenderer.h"
#include "StubResourceManager.h"
#include "StubTextureFactory.h"
#include "StubWindow.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/SceneFactory/SceneFactory.h"
#include "engine/include/TextureLoader/TextureLoader.h"
#include "engine/value_objects/GameSettings/GameSettings.h"

class StubSceneFactory : public SceneFactory
{
public:
    StubSceneFactory() : SceneFactory(SceneFactory::Config{
        .window = this->dummyWindow,
        .parser = this->dummyParser,
        .resourceManager = this->dummyResourceManager,
        .textureLoader = this->dummyTextureLoader,
        .renderer = this->dummyRenderer,
        .eventBus = this->dummyEventBus,
        .settings = this->dummySettings,
        .engine = this->dummyEngine,
        .fontFactory = this->dummyFontFactory,
        .textureFactory = this->dummyTextureFactory
    }) {}

private:
    StubWindow dummyWindow;
    StubDataParser dummyParser;
    StubResourceManager dummyResourceManager;
    TextureLoader dummyTextureLoader{this->dummyTextureFactory};
    StubRenderer dummyRenderer;
    EventBus dummyEventBus;
    GameSettings dummySettings;
    StubEngine dummyEngine{this->dummyWindow, this->dummySettings};
    StubFontFactory dummyFontFactory;
    StubTextureFactory dummyTextureFactory;
};

#endif // stub_scene_factory_h
