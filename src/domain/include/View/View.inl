#ifndef view_inl
#define view_inl

#include "View.h"

#include <array>

template <typename... Components>
View<Components...>::View(ComponentManager& componentManager) : manager(componentManager)
{
    std::array<IComponentStorage*, sizeof...(Components)> storages =
        { this->manager.storage<Components>()... };

    this->baseIndex = 0;
    this->baseStorage = storages[0];

    for (size_t i = 1; i < storages.size(); i++)
    {
        if (storages[i]->size() >= this->baseStorage->size()) continue;
        
        this->baseStorage = storages[i];
        this->baseIndex = i;
    }
}

template <typename... Components>
View<Components...>::Iterator::Iterator(
    ComponentManager& manager, const std::vector<Entity>& entities, size_t index, size_t baseIndex
) : manager(manager), entities(entities), index(index), baseIndex(baseIndex) { this->advance(); }

template <typename... Components>
typename View<Components...>::Iterator&
View<Components...>::Iterator::operator++()
{
    this->index++;
    this->advance();
    return *this;
}

template <typename... Components>
bool View<Components...>::Iterator::operator==(const Iterator& other) const
{ return this->index == other.index && &this->entities == &other.entities; }

template <typename... Components>
bool View<Components...>::Iterator::operator!=(const Iterator& other) const
{ return !(*this == other); }

template <typename... Components>
auto View<Components...>::Iterator::operator*()
{
    Entity e = this->entities[this->index];
    return std::tuple<Entity, Components&...>(e, this->manager.get<Components>(e)...);
}

template <typename... Components>
bool View<Components...>::Iterator::matches(Entity e)
{ return this->matchesImpl(e, std::index_sequence_for<Components...>{}); }

template <typename... Components>
template <size_t... I>
bool View<Components...>::Iterator::matchesImpl(Entity e, std::index_sequence<I...>)
{
    return ((
        I == this->baseIndex ||
        this->manager.has<std::tuple_element_t<I, std::tuple<Components...>>>(e)) && ...
    );
}

template <typename... Components>
void View<Components...>::Iterator::advance()
{
    while (this->index < this->entities.size())
    {
        Entity e = this->entities[this->index];

        if (this->matches(e)) break;
        this->index++;
    }
}

template <typename... Components>
typename View<Components...>::Iterator
View<Components...>::begin()
{ return Iterator(this->manager, this->baseStorage->entities(), 0, this->baseIndex); }

template <typename... Components>
typename View<Components...>::Iterator
View<Components...>::end()
{ return Iterator(this->manager, this->baseStorage->entities(), this->baseStorage->entities().size(), this->baseIndex); }

#endif // view_inl
