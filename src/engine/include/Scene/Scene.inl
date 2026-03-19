#include "Scene.h"

#include <utility>

template <typename T, typename... Args>
T& Scene::addSystem(Args&&... args) { return this->systems.addSystem<T>(std::forward<Args>(args)...); }
