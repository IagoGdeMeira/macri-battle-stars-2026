#ifndef scene_manager_h
#define scene_manager_h

#include "../Scene/Scene.h"
#include "../SceneFactory/SceneFactory.h"

#include <vector>
#include <memory>

class SceneManager
{
public:
    explicit SceneManager(SceneFactory& factory) : factory(factory) {}

    template <typename SceneType>
    void changeScene(typename SceneType::Config config);

    void pushScene(std::unique_ptr<Scene> scene);
    void popScene();

    Scene* currentScene() const { return this->sceneStack.empty() ? nullptr : this->sceneStack.back().get(); }

    void update(float deltaTime);
    void render() { for (auto& scene : this->sceneStack) scene->render(); }

private:
    SceneFactory& factory;
    std::vector<std::unique_ptr<Scene>> sceneStack;
};

#include "SceneManager.inl"

#endif // scene_manager_h
