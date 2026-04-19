#ifndef component_manager_inl
#define component_manager_inl

#include "ComponentManager.h"

#include <utility>

template <typename T>
void ComponentManager::registerComponent()
{
    uint32_t id = this->componentTypeId<T>();

    if (id >= this->storages.size()) this->storages.resize(id + 1);

    if (this->storages[id]) throw std::logic_error("Component already registered");

    this->storages[id] = std::make_unique<ComponentStorage<T>>();
}

template <typename T>
void ComponentManager::add(Entity entity, const T& component)
{ this->getStorage<T>()->add(entity, component); }

template <typename T>
void ComponentManager::add(Entity entity, T&& component)
{ this->getStorage<T>()->add(entity, std::move(component)); }

template <typename T>
void ComponentManager::remove(Entity entity) { this->getStorage<T>()->remove(entity); }

template <typename T>
bool ComponentManager::has(Entity entity) const { return this->getStorage<T>()->has(entity); }

template <typename T>
T& ComponentManager::get(Entity entity) { return this->getStorage<T>()->get(entity); }

template <typename T>
IComponentStorage* ComponentManager::storage() { return this->getStorage<T>(); }

template <typename T>
const ComponentStorage<T>* ComponentManager::storage() const { return this->getStorage<T>(); }

template <typename T>
uint32_t ComponentManager::componentTypeId()
{
    static uint32_t id = ComponentManager::nextComponentTypeId++;
    return id;
}

template <typename T>
ComponentStorage<T>* ComponentManager::getStorage()
{
    uint32_t id = this->componentTypeId<T>();

    if (id >= this->storages.size() || !this->storages[id])
    { throw std::logic_error("Component not registered"); }

    return static_cast<ComponentStorage<T>*>(this->storages[id].get());
}

template <typename T>
const ComponentStorage<T>* ComponentManager::getStorage() const
{
    uint32_t id = this->componentTypeId<T>();

    if (id >= this->storages.size() || !this->storages[id])
    { throw std::logic_error("Component not registered"); }

    return static_cast<const ComponentStorage<T>*>(this->storages[id].get());
}

#endif // component_manager_inl
