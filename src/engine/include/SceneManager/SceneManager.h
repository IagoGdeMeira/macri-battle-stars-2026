#ifndef scene_manager_h
#define scene_manager_h

#include <memory>

class Scene;

class SceneManager
{
public:
    SceneManager();
    ~SceneManager();

    template <typename T, typename... Args>    
    void changeScene(Args&&... args);

    void update(float deltaTime);

private:
    std::unique_ptr<Scene> currentScene;
    std::unique_ptr<Scene> nextScene;
};

#include "SceneManager.inl"

#endif // scene_manager_h
