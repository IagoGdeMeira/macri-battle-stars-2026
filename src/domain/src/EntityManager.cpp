#include "EntityManager/EntityManager.h"

Entity EntityManager::create()
{
    uint32_t id;

    if (!this->freeIds.empty())
    {
        id = this->freeIds.front();
        this->freeIds.pop();
    }
    else id = this->nextId++;

    if (id >= this->alive.size()) this->alive.resize(id + 1, EntityManager::DEAD);
    this->alive[id] = EntityManager::ALIVE;

    return Entity{id};
}

void EntityManager::destroy(Entity e)
{
    uint32_t id = e.id;
    
    if (id >= this->alive.size() || !this->alive[id]) return;

    this->alive[id] = EntityManager::DEAD;
    this->freeIds.push(id);
}

bool EntityManager::isAlive(Entity e) const
{
    uint32_t id = e.id;

    if (id >= this->alive.size()) return false;
    return this->alive[id] == EntityManager::ALIVE;
}

void EntityManager::clear()
{
    this->nextId = 0;
    std::queue<uint32_t> emptyQueue;
    std::swap(this->freeIds, emptyQueue);
    this->alive.clear();
}
