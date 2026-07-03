#ifndef ui_action_factory_h
#define ui_action_factory_h

#include "../include/IUIAction/IUIAction.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/SceneManager/SceneManager.h"

class UIActionFactory
{
public:
    struct Config { EventBus& eventBus; SceneManager& sceneManager; };

    explicit UIActionFactory(const Config& config) : eventBus(config.eventBus), sceneManager(config.sceneManager) {}

    std::unique_ptr<IUIAction> create(const std::string& type) const;

private:
    EventBus& eventBus;
    SceneManager& sceneManager;
};

#endif // ui_action_factory_h
