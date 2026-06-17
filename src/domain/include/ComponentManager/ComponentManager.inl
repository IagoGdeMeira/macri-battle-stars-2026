#ifndef component_manager_inl
#define component_manager_inl

#include "ComponentManager.h"

#include <utility>

template <typename T>
void ComponentManager::registerComponent()
{
    uint32_t id = ComponentManager::getTypeId<T>();
    if (id >= this->storages.size()) this->storages.resize(id + 1);
    if (this->storages[id]) return;
    this->storages[id] = std::make_unique<ComponentStorage<T>>();
}

template <typename T>
ComponentStorage<T>* ComponentManager::getStorage()
{
    uint32_t id = ComponentManager::getTypeId<T>();
    if (id >= this->storages.size() || !this->storages[id]) throw std::logic_error("Component not registered");
    return static_cast<ComponentStorage<T>*>(this->storages[id].get());
}

template <typename T>
const ComponentStorage<T>* ComponentManager::getStorage() const
{ return const_cast<ComponentManager*>(this)->getStorage<T>(); }

#endif // component_manager_inl
