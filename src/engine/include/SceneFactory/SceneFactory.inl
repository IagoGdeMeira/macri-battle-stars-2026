#ifndef scene_factory_inl
#define scene_factory_inl

#include "SceneFactory.h"

template <typename SceneType>
std::unique_ptr<Scene> SceneFactory::createScene(typename SceneType::Config cfg, SceneManager* sceneManager)
{
    cfg.eventBus        = &this->eventBus;
    cfg.sceneManager    = sceneManager;
    cfg.renderer        = &this->renderer;
    cfg.window          = &this->window;
    cfg.parser          = &this->parser;
    cfg.resourceManager = &this->resourceManager;
    cfg.textureLoader   = &this->textureLoader;
    cfg.settings        = &this->settings;
    cfg.engine          = &this->engine;
    cfg.fontFactory     = &this->fontFactory;
    cfg.textureFactory  = &this->textureFactory;

    return std::make_unique<SceneType>(std::move(cfg));
}

#endif // scene_factory_inl
