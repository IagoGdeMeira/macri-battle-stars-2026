#ifndef signature_manager_h
#define signature_manager_h

#include "../ComponentType/ComponentType.h"
#include "../Entity/Entity.h"

#include <cstddef>
#include <cstdint>
#include <vector>

class SignatureManager
{
public:
    template <typename Component>
    size_t getComponentIndex();

    void addComponent(Entity e, size_t componentIndex);
    void removeComponent(Entity e, size_t componentIndex);

    bool hasComponent(Entity e, size_t componentIndex) const;

    template <typename... Indices>
    bool match(Entity e, Indices... indices) const;

    template <typename... Components>
    bool matchTypes(Entity e) const;

    void clear(Entity e);

private:
    using Signature = std::vector<uint64_t>;
    using SignatureBlock = uint64_t;

    std::vector<Signature> signatures;

    static constexpr size_t BITS_PER_BLOCK = sizeof(SignatureBlock) * 8;

    void ensureCapacity(Entity e, size_t componentIndex);
    bool match(Entity e, const size_t *indices, size_t count) const;
};

#include "SignatureManager.inl"

#endif // signature_manager_h
