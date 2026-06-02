#ifndef scene_manager_inl
#define scene_manager_inl

#include "SceneManager.h"

#include "../SceneFactory/SceneFactory.h"

#include <stdexcept>
#include <utility>

template <typename SceneType>
void SceneManager::changeScene(typename SceneType::Config config)
{
    auto& stack = this->sceneStack;
    if (!stack.empty()) stack.back()->onExit();
    
    auto newScene = this->factory.createScene<SceneType>(std::move(config));

    stack.clear();
    stack.push_back(std::move(newScene));

    stack.back()->init();
    stack.back()->onEnter();
}

#endif // scene_manager_inl
