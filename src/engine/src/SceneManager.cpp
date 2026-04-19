#include "../include/SceneManager/SceneManager.h"

#include "../include/Scene/Scene.h"

void SceneManager::popScene()
{
    if (this->sceneStack.empty()) return;

    Scene* oldTop = this->sceneStack.back().get();
    oldTop->onExit();

    this->sceneStack.pop_back();
    if (!this->sceneStack.empty()) this->sceneStack.back()->onResume();
}

void SceneManager::update(float deltaTime)
{
    for (size_t i = 0; i < this->sceneStack.size(); ++i)
    {
        auto& scene = this->sceneStack[i];
        bool isTop = (i == this->sceneStack.size() - 1);
        Scene::UpdatePolicy policy = scene->getUpdatePolicy();

        bool shouldUpdate =
            (policy == Scene::UpdatePolicy::Always) ||
            (policy == Scene::UpdatePolicy::WhenTop && isTop);

        if (shouldUpdate) scene->update(deltaTime);
    }
}

std::unique_ptr<Scene> SceneManager::createScene(SceneId id, std::any data)
{
    auto scene = this->factory.createScene(id, std::move(data));
    if (!scene) throw std::runtime_error("SceneFactory returned nullptr for SceneId");
    
    return scene;
}
