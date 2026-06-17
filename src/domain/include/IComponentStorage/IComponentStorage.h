#ifndef i_component_storage_h
#define i_component_storage_h

#include "../Entity/Entity.h"

#include <vector>

class IComponentStorage
{
public:
    virtual ~IComponentStorage() = default;

    virtual void remove(Entity e) = 0;
    virtual bool has(Entity e) const = 0;
    virtual size_t size() const = 0;

    virtual const std::vector<Entity>& entities() const = 0;
};

#endif // i_component_storage_h
