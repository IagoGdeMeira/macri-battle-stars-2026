#include "../include/EntityManager/EntityManager.h"

Entity EntityManager::create()
{
    size_t id;

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

    ++this->generations[e.index()];
    this->freeIds.push_back(e.index());
}

bool EntityManager::isAlive(Entity e) const
{
    if (e.index() >= this->generations.size()) return false;
    return this->generations[e.index()] == e.gen();
}
