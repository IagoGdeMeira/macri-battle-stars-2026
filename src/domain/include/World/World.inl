#include "World.h"

template <typename Component>
void World::addComponent(Entity e, const Component &component)
{
    this->components.add<Component>(e, component);

    auto idx = this->signatures.getComponentIndex<Component>();
    this->signatures.addComponent(e, idx);
}

template <typename Component>
void World::removeComponent(Entity e)
{
    this->components.remove<Component>(e);

    auto idx = this->signatures.getComponentIndex<Component>();
    this->signatures.removeComponent(e, idx);
}

template <typename Component>
Component &World::getComponent(Entity e)
{ return this->components.get<Component>(e); }

template <typename Component>
bool World::hasComponent(Entity e) const
{ return this->components.has<Component>(e); }

template <typename... Components, typename Func>
void World::each(Func func)
{
    auto *storages[] = {this->components.findStorage<Components>()...};
    for (auto *s : storages) if (!s) return;

    auto *smallestStorage = *std::min_element(
        std::begin(storages),
        std::end(storages),
        [](auto *a, auto *b) { return a->size() < b->size(); });

    const auto &entitiesList = smallestStorage->getEntities();

    for (Entity e : entitiesList)
    {
        if (!this->entities.isAlive(e)) continue;

        if (!(this->signatures.hasComponent(
            e, this->signatures.getComponentIndex<Components>()) && ...)) continue;

        func(e, this->components.get<Components>(e)...);
    }
}
