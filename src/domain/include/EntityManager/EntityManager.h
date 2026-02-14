#ifndef entity_manager_h
#define entity_manager_h

#include "../Entity/Entity.h"

#include <cstdint>
#include <vector>

class EntityManager
{
public:
    Entity create();
    void destroy(Entity e);

    bool isAlive(Entity e) const;

private:
    uint32_t nextId = 0;
    std::vector<uint32_t> generations;
    std::vector<uint32_t> freeIds;
};

#endif // entity_manager_h
