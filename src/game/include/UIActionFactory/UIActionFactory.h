#ifndef ui_action_factory_h
#define ui_action_factory_h

#include "IUIAction/IUIAction.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/SceneManager/SceneManager.h"

#include <utility>

class UIActionFactory
{
public:
    struct Config { EventBus& eventBus; SceneManager& sceneManager; };

    explicit UIActionFactory(Config cfg) : cfg(std::move(cfg)) {}

    std::unique_ptr<IUIAction> create(const std::string& type) const;

private:
    Config cfg;
};

#endif // ui_action_factory_h
