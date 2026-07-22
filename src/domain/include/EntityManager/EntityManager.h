#ifndef entity_manager_h
#define entity_manager_h

#include "Entity/Entity.h"

#include <cstdint>
#include <queue>
#include <vector>

class EntityManager
{
public:
    Entity create();
    void destroy(Entity e);
    bool isAlive(Entity e) const;
    void clear();

private:
    static constexpr char DEAD = 0, ALIVE = 1;

    uint32_t nextId = 0;
    std::queue<uint32_t> freeIds;
    std::vector<char> alive;
};

#endif // entity_manager_h
