#include "SignatureManager.h"

template <typename Component>
size_t SignatureManager::getComponentIndex()
{ return ComponentType::id<Component>(); }

template <typename... Indices>
bool SignatureManager::match(Entity e, Indices... indices) const
{
    const size_t indexArray[] = {static_cast<size_t>(indices)...};
    return this->match(e, indexArray, sizeof...(indices));
}
