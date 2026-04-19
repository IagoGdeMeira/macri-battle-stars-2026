#ifndef scene_manager_inl
#define scene_manager_inl

#include <stdexcept>

template <typename Data>
void SceneManager::changeScene(SceneId id, Data &&data)
{
    if (!this->sceneStack.empty()) this->sceneStack.back()->onExit();

    this->sceneStack.clear();

    auto newScene = createScene(id, std::forward<Data>(data));
    this->sceneStack.push_back(std::move(newScene));
    this->sceneStack.back()->init();
    this->sceneStack.back()->onEnter();
}

template <typename Data>
void SceneManager::pushScene(SceneId id, Data&& data)
{
    if (!this->sceneStack.empty()) this->sceneStack.back()->onPause();

    auto newScene = createScene(id, std::forward<Data>(data));
    this->sceneStack.push_back(std::move(newScene));
    this->sceneStack.back()->init();
    this->sceneStack.back()->onEnter();
}

#endif // scene_manager_inl
