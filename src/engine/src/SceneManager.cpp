#include "SceneManager/SceneManager.h"

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
    if (stack.empty()) return;

    for (int i = static_cast<int>(stack.size()) - 1; i >= 0; --i)
    {
        const bool isTop = (i == static_cast<int>(stack.size()) - 1);

        if (isTop || stack[i + 1]->allowsUpdateBelow()) stack[i]->update(deltaTime);
        else break;
    }
}
