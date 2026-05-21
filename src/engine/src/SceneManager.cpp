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

        using Policy = Scene::UpdatePolicy;
        Policy policy = scene->getUpdatePolicy();

        if (policy == Policy::Always || (policy == Policy::WhenTop && isTop)) scene->update(deltaTime);
    }
}

std::unique_ptr<Scene> SceneManager::createScene(SceneId id, std::any data)
{
    auto scene = this->factory.createScene(id, std::move(data));
    if (!scene) throw std::runtime_error("SceneFactory returned nullptr for SceneId");
    
    return scene;
}

void SceneManager::startScene(std::unique_ptr<Scene> scene)
{
    this->sceneStack.push_back(std::move(scene));
    this->sceneStack.back()->init();
    this->sceneStack.back()->onEnter();
}
