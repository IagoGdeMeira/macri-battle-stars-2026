#ifndef scene_manager_h
#define scene_manager_h

#include "../ISceneFactory/ISceneFactory.h"
#include "../Scene/Scene.h"
#include "../SceneId/SceneId.h"

#include <any>
#include <memory>
#include <vector>

class SceneManager
{
public:
    explicit SceneManager(ISceneFactory& factory) : factory(factory) {}
    ~SceneManager() = default;

    template <typename Data>
    void changeScene(SceneId id, Data&& data);

    template <typename Data>
    void pushScene(SceneId id, Data&& data);
    
    void popScene();

    Scene* currentScene() { return this->sceneStack.empty() ? nullptr : this->sceneStack.back().get(); }
    bool empty() const { return this->sceneStack.empty(); }

    void update(float deltaTime);
    void render() { for (auto& scene : this->sceneStack) scene->render(); }

private:
    ISceneFactory& factory;
    std::vector<std::unique_ptr<Scene>> sceneStack;

    std::unique_ptr<Scene> createScene(SceneId id, std::any data);
};

#include "SceneManager.inl"

#endif // scene_manager_h
