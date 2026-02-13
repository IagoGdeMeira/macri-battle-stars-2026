#include "ComponentStorage.h"

#include <cstdint>
#include <stdexcept>

template <typename ComponentType>
void ComponentStorage<ComponentType>::add(Entity e, const ComponentType &component)
{
    this->entityToIndex[e.id] = this->components.size();
    this->entities.push_back(e);
    this->components.push_back(component);
}

template <typename ComponentType>
void ComponentStorage<ComponentType>::remove(Entity e)
{
    auto it = entityToIndex.find(e.id);
    if (it == entityToIndex.end()) return;
    
    auto index = it->second;
    auto last = this->components.size() - 1;

    this->components[index] = std::move(this->components[last]);
    this->entities[index] = this->entities[last];

    this->entityToIndex[this->entities[index].id] = index;

    this->components.pop_back();
    this->entities.pop_back();
    this->entityToIndex.erase(e.id);
}

template <typename ComponentType>
bool ComponentStorage<ComponentType>::has(Entity e) const
{ return this->entityToIndex.find(e.id) != this->entityToIndex.end(); }

template <typename ComponentType>
ComponentType &ComponentStorage<ComponentType>::get(Entity e)
{
    if (!this->entityToIndex.contains(e.id)) throw std::runtime_error("Component not found");
    return this->components[this->entityToIndex[e.id]];
}
