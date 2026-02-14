#include "../include/SignatureManager/SignatureManager.h"

void SignatureManager::addComponent(Entity e, size_t componentIndex)
{
    this->ensureCapacity(e);

    size_t block = componentIndex / 64;
    size_t bit = componentIndex % 64;

    this->signatures[e.id][block] |= (1ULL << bit);
}

void SignatureManager::removeComponent(Entity e, size_t componentIndex)
{
    if (e.id >= this->signatures.size()) return;

    size_t block = componentIndex / 64;
    size_t bit = componentIndex % 64;

    if (block >= this->signatures[e.id].size()) return;

    this->signatures[e.id][block] &= ~(1ULL << bit);
}

bool SignatureManager::hasComponent(Entity e, size_t componentIndex) const
{
    if (e.id >= this->signatures.size()) return false;

    size_t block = componentIndex / 64;
    size_t bit = componentIndex % 64;

    if (block >= this->signatures[e.id].size()) return false;

    return (this->signatures[e.id][block] & (1ULL << bit)) != 0;
}

void SignatureManager::clear(Entity e)
{
    if (e.id < this->signatures.size())
    { std::fill(this->signatures[e.id].begin(), this->signatures[e.id].end(), 0); }
}

void SignatureManager::ensureCapacity(Entity e)
{
    if (e.id >= this->signatures.size()) this->signatures.resize(e.id + 1);

    size_t requiredBlocks = (this->nextComponentIndex + 63) / 64;

    if (this->signatures[e.id].size() < requiredBlocks)
    { this->signatures[e.id].resize(requiredBlocks, 0); }
}
