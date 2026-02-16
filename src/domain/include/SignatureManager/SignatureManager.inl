#include "SignatureManager.h"

template <typename Component>
size_t SignatureManager::getComponentIndex() { return ComponentType::id<Component>(); }

template <typename... Indices>
bool SignatureManager::match(Entity e, Indices... indices) const
{
    static_assert(sizeof...(indices) > 0, "match requires at least one component index");

    const size_t indexArray[] = {static_cast<size_t>(indices)...};
    return this->match(e, indexArray, sizeof...(indices));
}

template <typename... Components>
bool SignatureManager::matchTypes(Entity e) const
{
    static_assert(sizeof...(Components) > 0, "matchTypes requires at least one component type");

    return this->match(e, ComponentType::id<Components>()...);
}
