#include "../include/SceneManager/SceneManager.h"

void SceneManager::pushScene(std::unique_ptr<Scene> scene)
{
    auto& stack = this->sceneStack;
    if (!stack.empty()) stack.back()->onPause();

    stack.push_back(std::move(scene));
    stack.back()->init();
    stack.back()->onEnter();
}

void SceneManager::popScene()
{
    auto& stack = this->sceneStack;
    if (stack.empty()) return;

    stack.back()->onExit();
    stack.pop_back();

    if (!stack.empty()) stack.back()->onResume();
}

void SceneManager::update(float deltaTime)
{
    auto& stack = this->sceneStack;
    for (size_t i = 0; i < stack.size(); ++i)
    {
        auto& scene = stack[i];
        bool isTop = (i == stack.size() - 1);

        using Policy = Scene::UpdatePolicy;
        Policy policy = scene->getUpdatePolicy();

        bool shouldUpdate = (policy == Policy::Always) || (policy == Policy::WhenTop && isTop);
        if (shouldUpdate) scene->update(deltaTime);
    }
}
