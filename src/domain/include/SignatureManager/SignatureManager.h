#ifndef signature_manager_h
#define signature_manager_h

#include "../Entity/Entity.h"

#include <cstddef>
#include <cstdint>
#include <typeindex>
#include <unordered_map>
#include <vector>

class SignatureManager
{
public:
    template <typename Component>
    size_t getComponentIndex();

    void addComponent(Entity e, size_t componentIndex);
    void removeComponent(Entity e, size_t componentIndex);

    bool hasComponent(Entity e, size_t componentIndex) const;

    template <typename... Indexes>
    bool match(Entity e, Indexes... indexes) const;

    void clear(Entity e);

private:
    using Signature = std::vector<uint64_t>;

    std::vector<Signature> signatures;
    std::unordered_map<std::type_index, size_t> typeToIndex;
    size_t nextComponentIndex = 0;

    void ensureCapacity(Entity e);
};

#include "SignatureManager.inl"

#endif // signature_manager_h
