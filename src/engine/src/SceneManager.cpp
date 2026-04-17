#include "../include/SceneManager/SceneManager.h"

#include "../include/Scene/Scene.h"

void SceneManager::update(float deltaTime)
{
    if (this->nextScene)
    {
        this->currentScene = std::move(this->nextScene);
        this->currentScene->init();
    }

    if (this->currentScene) this->currentScene->update(deltaTime);
}
