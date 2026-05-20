#ifndef i_ui_action_h
#define i_ui_action_h

class SceneManager;

class IUIAction
{
public:
    virtual ~IUIAction() = default;

    virtual void execute(SceneManager& sceneManager) const = 0;
};

#endif // i_ui_action_h