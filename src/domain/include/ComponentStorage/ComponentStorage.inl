#ifndef component_storage_inl
#define component_storage_inl

#include "ComponentStorage.h"

#include <cassert>
#include <utility>

template<typename T>
void ComponentStorage<T>::add(Entity entity, const T& component)
{
    uint32_t id = entity.id;

    if (id >= this->sparse.size()) this->sparse.resize(id + 1, UINT32_MAX);
        
    assert(!this->has(entity) && "Component already exists");

    uint32_t index = static_cast<uint32_t>(this->denseComponents.size());

    this->denseEntities.push_back(entity);
    this->denseComponents.push_back(component);

    this->sparse[id] = index;
}

template<typename T>
void ComponentStorage<T>::add(Entity entity, T&& component)
{
    uint32_t id = entity.id;

    if (id >= this->sparse.size()) this->sparse.resize(id + 1, UINT32_MAX);

    assert(!this->has(entity) && "Component already exists");

    uint32_t index = static_cast<uint32_t>(this->denseComponents.size());

    this->denseEntities.push_back(entity);
    this->denseComponents.push_back(std::move(component));

    this->sparse[id] = index;
}

template <typename T>
void ComponentStorage<T>::remove(Entity entity)
{
    if (!this->has(entity)) return;

    uint32_t id = entity.id;

    uint32_t index = this->sparse[id];
    uint32_t lastIndex = static_cast<uint32_t>(this->denseComponents.size() - 1);

    if (index != lastIndex)
    {
        Entity lastEntity = this->denseEntities[lastIndex];

        this->denseEntities[index] = lastEntity;
        this->denseComponents[index] = std::move(this->denseComponents[lastIndex]);

        this->sparse[lastEntity.id] = index;
    }

    this->denseEntities.pop_back();
    this->denseComponents.pop_back();
    this->sparse[id] = UINT32_MAX;
}

template <typename T>
bool ComponentStorage<T>::has(Entity entity) const
{
    uint32_t id = entity.id;

    if (id >= this->sparse.size()) return false;

    uint32_t index = this->sparse[id];

    if (index == UINT32_MAX) return false;

    return index < this->denseEntities.size()
        && this->denseEntities[index].id == id;
}

template <typename T>
T& ComponentStorage<T>::get(Entity entity)
{
    assert(this->has(entity) && "Component does not exist");

    return this->denseComponents[this->sparse[entity.id]];
}

template <typename T>
const T& ComponentStorage<T>::get(Entity entity) const
{
    assert(this->has(entity) && "Component does not exist");

    return this->denseComponents[this->sparse[entity.id]];
}

template <typename T>
size_t ComponentStorage<T>::size() const { return this->denseComponents.size(); }

template <typename T>
const std::vector<Entity>& ComponentStorage<T>::entities() const { return this->denseEntities; }

#endif // component_storage_inl
