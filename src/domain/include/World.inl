#include "World.h"

template <typename ComponentType>
void World::addComponent(Entity entity, const ComponentType &component)
{ this->getOrCreateStorage<ComponentType>()->add(entity, component); }

template <typename ComponentType>
void World::removeComponent(Entity entity)
{
    auto* storage = this->findStorage<ComponentType>();
    if (!storage) return;

    storage->remove(entity);
}

template <typename ComponentType>
bool World::hasComponent(Entity entity) const
{
    auto *storage = this->findStorage<ComponentType>();
    return storage && storage->has(entity);
}

template <typename ComponentType>
ComponentType &World::getComponent(Entity entity)
{
    auto *storage = this->findStorage<ComponentType>();

    assert(storage && "Storage does not exist");
    assert(storage->has(entity) && "Entity does not have component");

    return storage->get(entity);
}

template <typename... Components, typename Func>
void World::each(Func func)
{
    auto storagesTuple = std::tuple<ComponentStorage<Components> *...>(this->findStorage<Components>()...);
    bool missing = false;

    std::apply([&](auto *...storages)
        { ((missing |= (storages == nullptr)), ...); }, storagesTuple);

    if (missing) return;

    size_t smallestIndex = 0;
    size_t smallestSize = SIZE_MAX;
    size_t index = 0;

    std::apply([&](auto *...storages)
        {
            (([&]
                {
                    if (storages->size() < smallestSize)
                    {
                        smallestSize = storages->size();
                        smallestIndex = index;
                    }
                    ++index; }()), ...);
        }, storagesTuple);

    std::apply(
        [&](auto *...storages)
        {
            auto storageArray = std::array{storages...};
            auto *smallest = storageArray[smallestIndex];

            for (const Entity &e : smallest->entities())
            { if ((storages->has(e) && ...)) func(e, storages->get(e)...); }
        }, storagesTuple);
}

template <typename ComponentType>
ComponentStorage<ComponentType> *World::findStorage()
{
    std::type_index index(typeid(ComponentType));

    auto it = this->storages.find(index);
    if (it == this->storages.end()) return nullptr;

    return static_cast<ComponentStorage<ComponentType> *>(it->second.get());
}

template <typename ComponentType>
const ComponentStorage<ComponentType> *World::findStorage() const
{
    std::type_index index(typeid(ComponentType));

    auto it = this->storages.find(index);
    if (it == this->storages.end()) return nullptr;

    return static_cast<const ComponentStorage<ComponentType> *>(it->second.get());
}

template <typename ComponentType>
ComponentStorage<ComponentType> *World::getOrCreateStorage()
{
    std::type_index index(typeid(ComponentType));

    auto it = this->storages.find(index);
    if (it != this->storages.end()) return static_cast<ComponentStorage<ComponentType> *>(it->second.get());

    auto storage = std::make_unique<ComponentStorage<ComponentType>>();
    ComponentStorage<ComponentType> *storagePtr = storage.get();
    this->storages[index] = std::move(storage);
    return storagePtr;
}
