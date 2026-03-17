#include "SceneManager.h"

#include <utility>

template <typename T, typename... Args>
void SceneManager::changeScene(Args&&... args)
{ this->nextScene = std::make_unique<T>(std::forward<Args>(args)...); }
