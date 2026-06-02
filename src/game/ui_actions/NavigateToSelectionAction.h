#ifndef navigate_to_selection_action_h
#define navigate_to_selection_action_h

#include "../include/IUIAction/IUIAction.h"
#include "../scenes/SelectionScene.h"

#include "../../engine/include/SceneManager/SceneManager.h"

class NavigateToSelectionAction : public IUIAction
{
public:
    explicit NavigateToSelectionAction(SceneManager& sm) : sceneManager(sm) {}
    void execute() const override { this->sceneManager.changeScene(SceneId::Selection, nullptr); }

private:
    SceneManager& sceneManager;
};

#endif // navigate_to_selection_action_h
