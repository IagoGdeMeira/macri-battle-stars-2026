#include "ResourceManager/ResourceManager.h"

void ResourceManager::clearCache()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->resources.clear();
    this->loading.clear();
    ++this->version;
}

void ResourceManager::sweep()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    for (auto it = this->resources.begin(); it != this->resources.end(); )
    {
        if (it->second.expired()) it = this->resources.erase(it);
        else ++it;
    }
}
