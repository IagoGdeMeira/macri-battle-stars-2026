#ifndef scene_factory_inl
#define scene_factory_inl

#include "SceneFactory.h"

template <typename SceneType>
std::unique_ptr<Scene> SceneFactory::createScene(typename SceneType::Config cfg, SceneManager* sceneManager)
{
    cfg.eventBus        = &this->cfg.eventBus;
    cfg.sceneManager    = sceneManager;
    cfg.renderer        = &this->cfg.renderer;
    cfg.window          = &this->cfg.window;
    cfg.parser          = &this->cfg.parser;
    cfg.resourceManager = &this->cfg.resourceManager;
    cfg.textureLoader   = &this->cfg.textureLoader;
    cfg.settings        = &this->cfg.settings;
    cfg.engine          = &this->cfg.engine;
    cfg.fontFactory     = &this->cfg.fontFactory;
    cfg.textureFactory  = &this->cfg.textureFactory;
    cfg.platformFactory = this->cfg.platformFactory;

    return std::make_unique<SceneType>(std::move(cfg));
}

#endif // scene_factory_inl
