#include "../include/UIActionFactory/UIActionFactory.h"

#include "../ui_actions/NavigateToSelectionAction.h"
#include "../ui_actions/QuitAction.h"

#include <stdexcept>

std::unique_ptr<IUIAction> UIActionFactory::create(const std::string& type) const
{
    if (type == "NavigateToSelection")  return std::make_unique<NavigateToSelectionAction>(this->sceneManager);
    if (type == "Quit")                 return std::make_unique<QuitAction>(this->eventBus);

    throw std::runtime_error("Unknown UI action type: " + type);
}
