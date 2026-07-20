#ifndef scene_manager_h
#define scene_manager_h

#include "Scene/Scene.h"

#include <memory>
#include <vector>

class SceneFactory;
class Engine;

class SceneManager
{
public:
    SceneManager(SceneFactory& factory, Engine& engine) : factory(factory), engine(engine) {}

    template <typename SceneType>
    void changeScene(typename SceneType::Config config);

    void pushScene(std::unique_ptr<Scene> scene);
    void popScene();

    Scene* currentScene() const { return this->sceneStack.empty() ? nullptr : this->sceneStack.back().get(); }

    virtual void update(float deltaTime);
    void render() { for (auto& scene : this->sceneStack) scene->render(); }

    Engine& getEngine() { return this->engine; }

private:
    SceneFactory& factory;
    Engine& engine;
    std::vector<std::unique_ptr<Scene>> sceneStack;
};

#include "SceneManager.inl"

#endif // scene_manager_h
