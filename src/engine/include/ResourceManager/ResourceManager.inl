#ifndef resource_manager_inl
#define resource_manager_inl

#include "ResourceManager.h"

#include <typeinfo>

template<typename T>
std::shared_ptr<T> ResourceManager::load(ResourceLoader<T>& loader, const std::string& path)
{
    auto key = this->makeKey<T>(path);

    std::lock_guard<std::mutex> lock(this->mutex);

    auto it = this->resources.find(key);
    if (it != this->resources.end()) if (auto existing = it->second.lock()) return std::static_pointer_cast<T>(existing);

    auto resource = this->sync.load(loader, path);
    if (resource) this->resources[key] = resource;

    return resource;
}

template<typename T>
std::future<std::shared_ptr<T>> ResourceManager::loadAsync(ResourceLoader<T>& loader, const std::string& path)
{
    auto key = this->makeKey<T>(path);

    std::lock_guard<std::mutex> lock(this->mutex);

    auto it = this->resources.find(key);
    if (it != this->resources.end()) if (auto existing = it->second.lock())
    {  
        std::promise<std::shared_ptr<T>> promise;
        promise.set_value(std::static_pointer_cast<T>(existing));
        return promise.get_future();
    }

    auto itLoading = this->loading.find(key);
    if (itLoading != this->loading.end()) return this->wrapFuture<T>(itLoading->second);

    auto promise = std::make_shared<std::promise<std::shared_ptr<void>>>();
    auto sharedFuture = promise->get_future().share();
    this->loading[key] = sharedFuture;

    auto currentVersion = this->version.load();

    this->async.enqueueTask([this, &loader, path, key, promise, currentVersion]()
    {
        auto resource = loader.load(path);

        {
            std::lock_guard<std::mutex> lk(this->mutex);
            if (this->version.load() == currentVersion && resource) this->resources[key] = resource;
            this->loading.erase(key);
        }

        promise->set_value(resource);
    });

    return this->wrapFuture<T>(sharedFuture);
}

template<typename T>
std::string ResourceManager::makeKey(const std::string& path) { return std::string(typeid(T).name()) + ":" + path; }

template<typename T>
std::future<std::shared_ptr<T>> ResourceManager::wrapFuture(std::shared_future<std::shared_ptr<void>> future)
{
    auto promise = std::make_shared<std::promise<std::shared_ptr<T>>>();

    this->async.enqueueTask([future, promise]() mutable {
        auto rawVoid = future.get();
        promise->set_value(std::static_pointer_cast<T>(rawVoid));
    });

    return promise->get_future();
}

#endif // resource_manager_inl
