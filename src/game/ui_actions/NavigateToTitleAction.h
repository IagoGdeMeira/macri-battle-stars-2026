#ifndef navigato_to_title_action_h
#define navigato_to_title_action_h

#include "../include/IUIAction/IUIAction.h"
#include "../scenes/TitleScene.h"

#include "../../engine/include/SceneManager/SceneManager.h"

class NavigateToTitleAction : public IUIAction
{
public:
    explicit NavigateToTitleAction(SceneManager& sceneManager) : sceneManager(sceneManager) {}
    void execute() const override { this->sceneManager.changeScene(SceneId::Title, nullptr); }

private:
    SceneManager& sceneManager;
};

#endif // navigato_to_title_action_h
