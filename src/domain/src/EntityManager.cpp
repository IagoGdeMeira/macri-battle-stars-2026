#include "../include/EntityManager/EntityManager.h"

Entity EntityManager::create()
{
    uint32_t id;

    if (!this->freeIds.empty())
    {
        id = this->freeIds.back();
        this->freeIds.pop_back();
    }
    else
    {
        id = this->nextId++;
        this->generations.resize(id + 1, 0);
    }

    return Entity{id, this->generations[id]};
}

void EntityManager::destroy(Entity e)
{
    if (!this->isAlive(e)) return;

    ++this->generations[e.id];
    this->freeIds.push_back(e.id);
}

bool EntityManager::isAlive(Entity e) const
{
    if (e.id >= this->generations.size()) return false;
    return this->generations[e.id] == e.generation;
}
