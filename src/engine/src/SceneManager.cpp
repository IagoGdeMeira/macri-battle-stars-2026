#include "SceneManager/SceneManager.h"

#include "domain/utils/Logger/Logger.h"

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
    LOG_DEBUG("SceneManager::update called, stack size = {}", this->sceneStack.size());
    auto& stack = this->sceneStack;
    for (size_t i = 0; i < stack.size(); ++i)
    {
        auto& scene = stack[i];
        bool isTop = (i == stack.size() - 1);
        LOG_DEBUG("SceneManager: scene {} (isTop={})", i, isTop);

        using Policy = Scene::UpdatePolicy;
        Policy policy = scene->getUpdatePolicy();

        bool shouldUpdate = (policy == Policy::Always) || (policy == Policy::WhenTop && isTop);
        LOG_DEBUG("SceneManager: policy={}, shouldUpdate={}", static_cast<int>(policy), shouldUpdate);

        if (shouldUpdate)
        {
            LOG_DEBUG("SceneManager: calling scene->update");
            scene->update(deltaTime);
        }
    }
}
