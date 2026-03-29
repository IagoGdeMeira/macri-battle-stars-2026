#include "Scene.h"

#include <utility>

template <typename T, typename... Args>
T& Scene::addSystem(Args&&... args)
{ return this->systemManager.addSystem<T>(std::forward<Args>(args)...); }
