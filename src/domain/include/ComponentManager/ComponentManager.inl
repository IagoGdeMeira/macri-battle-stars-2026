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

template<typename Component>
Component& ComponentManager::get(Entity e)
{
    auto* storage = this->findStorage<Component>();
    assert(storage);
    return storage->get(e);
}

template<typename Component>
bool ComponentManager::has(Entity e) const
{
    auto* storage = this->findStorage<Component>();
    return storage && storage->has(e);
}

template<typename Component>
ComponentStorage<Component>* ComponentManager::findStorage()
{
    std::type_index index(typeid(Component));
    auto it = this->storages.find(index);
    if (it == this->storages.end()) return nullptr;
    return static_cast<ComponentStorage<Component>*>(it->second.get());
}

template<typename Component>
const ComponentStorage<Component>* ComponentManager::findStorage() const
{
    std::type_index index(typeid(Component));
    auto it = this->storages.find(index);

    if (it == this->storages.end()) return nullptr;
    return static_cast<const ComponentStorage<Component>*>(it->second.get());
}

template<typename Component>
ComponentStorage<Component>* ComponentManager::getOrCreateStorage()
{
    std::type_index index(typeid(Component));

    auto it = this->storages.find(index);
    if (it != this->storages.end())
    { return static_cast<ComponentStorage<Component>*>(it->second.get()); }

    auto storage = std::make_unique<ComponentStorage<Component>>();
    auto* ptr = storage.get();
    this->storages[index] = std::move(storage);
    return ptr;
}