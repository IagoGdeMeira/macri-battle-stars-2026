#include "ComponentManager.h"

#include <cassert>

template<typename Component>
void ComponentManager::add(Entity e, const Component& component)
{ this->getOrCreateStorage<Component>()->add(e, component); }

template<typename Component>
void ComponentManager::remove(Entity e)
{
    auto* storage = this->findStorage<Component>();
    if (storage) storage->remove(e);
}

template <typename Component>
Component &ComponentManager::get(Entity e)
{
    auto *storage = this->findStorage<Component>();
    if (!storage) throw std::runtime_error("Component storage not found");

    return storage->get(e);
}

template<typename Component>
bool ComponentManager::has(Entity e) const
{
    auto* storage = this->findStorage<Component>();
    return storage && storage->has(e);
}

template <typename Component>
ComponentStorage<Component> *ComponentManager::findStorage()
{
    size_t id = ComponentType::id<Component>();
    if (id >= this->storages.size()) return nullptr;

    return static_cast<ComponentStorage<Component> *>(this->storages[id].get());
}

template<typename Component>
const ComponentStorage<Component> *ComponentManager::findStorage() const
{
    size_t id = ComponentType::id<Component>();
    if (id >= this->storages.size()) return nullptr;
    
    return static_cast<ComponentStorage<Component> *>(this->storages[id].get());
}

template <typename Component>
ComponentStorage<Component> *ComponentManager::getOrCreateStorage()
{
    size_t id = ComponentType::id<Component>();

    if (id >= this->storages.size()) this->storages.resize(id + 1);
    if (!this->storages[id]) this->storages[id] = std::make_unique<ComponentStorage<Component>>();

    return static_cast<ComponentStorage<Component> *>(this->storages[id].get());
}
