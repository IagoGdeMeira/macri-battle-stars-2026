#include "World.h"

template <typename Component>
void World::addComponent(Entity e, const Component &component)
{
    if (!this->entities.isAlive(e)) throw std::runtime_error("Cannot add component to dead entity");

    this->components.add<Component>(e, component);

    auto idx = this->signatures.getComponentIndex<Component>();
    this->signatures.addComponent(e, idx);
}

template <typename Component>
void World::removeComponent(Entity e)
{
    if (!this->entities.isAlive(e)) throw std::runtime_error("Cannot remove component from dead entity");

    this->components.remove<Component>(e);

    auto idx = this->signatures.getComponentIndex<Component>();
    this->signatures.removeComponent(e, idx);
}

template <typename Component>
Component &World::getComponent(Entity e)
{
    if (!this->entities.isAlive(e)) throw std::runtime_error("Cannot get component from dead entity");

    return this->components.get<Component>(e);
}

template <typename Component>
bool World::hasComponent(Entity e) const
{
    if (!this->entities.isAlive(e)) return false;

    return this->components.has<Component>(e);
}

template <typename... Components, typename Func>
void World::each(Func func)
{
    auto *firstStorage = this->components.findStorage<
        std::tuple_element_t<0, std::tuple<Components...>>>();

    if (!firstStorage)
        return;

    const auto &entitiesList = firstStorage->getEntities();

    for (Entity e : entitiesList)
    {
        if (!this->entities.isAlive(e))
            continue;

        if (!(this->hasComponent<Components>(e) && ...))
            continue;

        func(e, this->components.get<Components>(e)...);
    }
}
