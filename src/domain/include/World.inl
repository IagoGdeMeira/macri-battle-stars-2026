#include "World.h"

template <typename ComponentType>
void World::addComponent(Entity entity, const ComponentType &component)
{ this->getStorage<ComponentType>()->add(entity, component); }

template <typename ComponentType>
void World::removeComponent(Entity entity)
{ this->getStorage<ComponentType>()->remove(entity); }

template <typename ComponentType>
bool World::hasComponent(Entity entity) const
{
    std::type_index index(typeid(ComponentType));

    auto it = this->storages.find(index);
    if (it == this->storages.end()) return false;

    auto *storage = static_cast<ComponentStorage<ComponentType> *>(it->second.get());
    return storage->has(entity);
}

template <typename ComponentType>
ComponentType &World::getComponent(Entity entity)
{ return this->getStorage<ComponentType>()->get(entity); }

template <typename ComponentType>
ComponentStorage<ComponentType> *World::getStorage()
{
    std::type_index index(typeid(ComponentType));

    if (this->storages.find(index) == this->storages.end())
    { this->storages[index] = std::make_unique<ComponentStorage<ComponentType>>(); }

    return static_cast<ComponentStorage<ComponentType> *>(this->storages[index].get());
}
