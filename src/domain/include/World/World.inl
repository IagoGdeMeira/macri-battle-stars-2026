#include "World.h"

inline Entity World::create() { return this->entityManager.create(); }

inline void World::destroy(Entity entity) { this->entityManager.destroy(entity); }

template <typename T>
void World::registerComponent() { this->componentManager.registerComponent<T>(); }

template <typename T>
void World::add(Entity entity, const T& component) { this->componentManager.add(entity, component); }

template <typename T>
void World::remove(Entity entity) { this->componentManager.remove<T>(entity); }

template <typename T>
bool World::has(Entity entity) const { return this->componentManager.has<T>(entity); }

template <typename T>
T& World::get(Entity entity) { return this->componentManager.get<T>(entity); }

template <typename... Components>
View<Components...> World::view() { return View<Components...>(this->componentManager); }
