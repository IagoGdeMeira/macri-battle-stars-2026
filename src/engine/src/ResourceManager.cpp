#include "ResourceManager/ResourceManager.h"

void ResourceManager::clearCache()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->resources.clear();
    this->loading.clear();
}
