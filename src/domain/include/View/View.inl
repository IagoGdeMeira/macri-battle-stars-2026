#ifndef view_inl
#define view_inl

#include "View.h"

#include <array>

template <typename... Components>
View<Components...>::View(ComponentManager& componentManager) : manager(componentManager)
{
    std::array<IComponentStorage*, sizeof...(Components)> storages = { this->manager.storage<Components>()... };

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
typename View<Components...>::Iterator&
View<Components...>::Iterator::operator++()
{
    this->cfg.index++;
    this->advance();
    return *this;
}

template <typename... Components>
bool View<Components...>::Iterator::operator==(const Iterator& other) const
{ return this->cfg.index == other.cfg.index && &this->cfg.entities == &other.cfg.entities; }

template <typename... Components>
bool View<Components...>::Iterator::operator!=(const Iterator& other) const { return !(*this == other); }

template <typename... Components>
auto View<Components...>::Iterator::operator*()
{
    Entity e = this->cfg.entities[this->cfg.index];
    return std::tuple<Entity, Components&...>(e, this->cfg.manager.get<Components>(e)...);
}

template <typename... Components>
bool View<Components...>::Iterator::matches(Entity e)
{ return this->matchesImpl(e, std::index_sequence_for<Components...>{}); }

template <typename... Components>
template <size_t... I>
bool View<Components...>::Iterator::matchesImpl(Entity e, std::index_sequence<I...>)
{
    return (... && (
        I == this->cfg.baseIndex ||
        this->cfg.manager.has<std::tuple_element_t<I, std::tuple<Components...>>>(e)
    ));
}

template <typename... Components>
void View<Components...>::Iterator::advance()
{
    while (this->cfg.index < this->cfg.entities.size())
    {
        Entity e = this->cfg.entities[this->cfg.index];
        if (this->matches(e)) break;
        this->cfg.index++;
    }
}

template <typename... Components>
size_t View<Components...>::size() const
{
    const auto& entities = this->baseStorage->entities();
    size_t count = 0;

    for (const Entity& e : entities) if (this->matchesEntity(e)) ++count;
    return count;
}

template <typename... Components>
bool View<Components...>::matchesEntity(Entity e) const
{
    std::array<IComponentStorage*, sizeof...(Components)> storages = {
        const_cast<ComponentManager&>(this->manager).storage<Components>()...};
    for (size_t i = 0; i < storages.size(); ++i)
    {
        if (i == this->baseIndex) continue;
        if (!storages[i]->has(e)) return false;
    }
    return true;
}

#endif // view_inl
