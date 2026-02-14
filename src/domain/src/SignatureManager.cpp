#include "../include/SignatureManager/SignatureManager.h"

void SignatureManager::addComponent(Entity e, size_t componentIndex)
{
    this->ensureCapacity(e, componentIndex);

    size_t block = componentIndex / SignatureManager::BITS_PER_BLOCK;
    size_t bit = componentIndex % SignatureManager::BITS_PER_BLOCK;

    this->signatures[e.index()][block] |= (1ULL << bit);
}

void SignatureManager::removeComponent(Entity e, size_t componentIndex)
{
    if (e.index() >= this->signatures.size()) return;

    size_t block = componentIndex / SignatureManager::BITS_PER_BLOCK;
    size_t bit = componentIndex % SignatureManager::BITS_PER_BLOCK;

    if (block >= this->signatures[e.index()].size()) return;

    this->signatures[e.index()][block] &= ~(1ULL << bit);
}

bool SignatureManager::hasComponent(Entity e, size_t componentIndex) const
{
    if (e.index() >= this->signatures.size()) return false;

    size_t block = componentIndex / SignatureManager::BITS_PER_BLOCK;
    size_t bit = componentIndex % SignatureManager::BITS_PER_BLOCK;

    if (block >= this->signatures[e.index()].size()) return false;

    return (this->signatures[e.index()][block] & (1ULL << bit)) != 0;
}

void SignatureManager::clear(Entity e)
{
    if (e.index() < this->signatures.size())
    { std::fill(this->signatures[e.index()].begin(), this->signatures[e.index()].end(), 0); }
}

void SignatureManager::ensureCapacity(Entity e, size_t componentIndex)
{
    if (e.index() >= this->signatures.size()) this->signatures.resize(e.index() + 1);

    size_t requiredBlocks = (componentIndex / SignatureManager::BITS_PER_BLOCK) + 1;

    if (this->signatures[e.index()].size() < requiredBlocks)
        this->signatures[e.index()].resize(requiredBlocks, 0);
}

bool SignatureManager::match(Entity e, const size_t *indices, size_t count) const
{
    if (e.index() >= this->signatures.size()) return false;
    const auto &signature = this->signatures[e.index()];

    for (size_t i = 0; i < count; ++i)
    {
        size_t index = indices[i];

        size_t block = index / SignatureManager::BITS_PER_BLOCK;
        size_t bit = index % SignatureManager::BITS_PER_BLOCK;

        if (block >= signature.size()) return false;
        if (!(signature[block] & (1ULL << bit))) return false;
    }
    return true;
}
