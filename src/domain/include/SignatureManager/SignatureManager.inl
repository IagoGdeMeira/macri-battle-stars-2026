#include "SignatureManager.h"

template <typename Component>
size_t SignatureManager::getComponentIndex()
{
    std::type_index index(typeid(Component));

    auto it = this->typeToIndex.find(index);
    if (it != this->typeToIndex.end()) return it->second;

    size_t newIndex = this->nextComponentIndex++;
    this->typeToIndex[index] = newIndex;

    return newIndex;
}

template <typename... Indexes>
bool SignatureManager::match(Entity e, Indexes... indexes) const
{ return (this->hasComponent(e, indexes) && ...); }
