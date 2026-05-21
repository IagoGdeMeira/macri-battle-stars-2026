#ifndef scene_manager_inl
#define scene_manager_inl

#include <stdexcept>

template <typename Data>
void SceneManager::changeScene(SceneId id, Data &&data)
{
    if (!this->sceneStack.empty()) this->sceneStack.back()->onExit();

    this->sceneStack.clear();

    auto newScene = this->createScene(id, std::forward<Data>(data));
    this->startScene(std::move(newScene));
}

template <typename Data>
void SceneManager::pushScene(SceneId id, Data&& data)
{
    if (!this->sceneStack.empty()) this->sceneStack.back()->onPause();

    auto newScene = this->createScene(id, std::forward<Data>(data));
    this->startScene(std::move(newScene));
}

#endif // scene_manager_inl
