#include "ResourceManager.h"

#include <typeinfo>

template<typename T>
std::shared_ptr<T> ResourceManager::load(
    ResourceLoader<T>& loader,
    const std::string& path
) {
    auto key = this->makeKey<T>(path);

    auto it = this->resources.find(key);
    if (it != this->resources.end()) if (auto existing = it->second.lock())
    { return std::static_pointer_cast<T>(existing); }
    
    auto resource = this->sync.load(loader, path);
    this->resources[key] = resource;

    return resource;
}

template<typename T>
std::future<std::shared_ptr<T>> ResourceManager::loadAsync(
    ResourceLoader<T>& loader,
    const std::string& path
) {
    auto key = this->makeKey<T>(path);

    {
        std::lock_guard<std::mutex> lock(this->mutex);

        auto it = resources.find(key);
        if (it != resources.end()) if (auto existing = it->second.lock())
        { return this->makeReadyFuture(std::static_pointer_cast<T>(existing)); }

        auto itLoading = loading.find(key);
        if (itLoading != loading.end()) return this->wrapFuture<T>(itLoading->second);
    }

    auto future = this->async.load(loader, path);
    auto sharedFuture = future.share();

    auto erasedFuture = std::shared_future<std::shared_ptr<void>>(
        std::async(std::launch::deferred, [sharedFuture]()
        { return std::static_pointer_cast<void>(sharedFuture.get()); }));

    {
        std::lock_guard<std::mutex> lock(this->mutex);
        this->loading[key] = erasedFuture;
    }

    return std::async(std::launch::deferred, [this, key, sharedFuture]()
    {
        auto resource = sharedFuture.get();

        {
            std::lock_guard<std::mutex> lock(this->mutex);
            this->resources[key] = resource;
            this->loading.erase(key);
        }

        return resource;
    });
}

template<typename T>
std::string ResourceManager::makeKey(const std::string& path)
{ return std::string(typeid(T).name()) + ":" + path; }

template<typename T>
std::future<std::shared_ptr<T>> ResourceManager::makeReadyFuture(
    std::shared_ptr<T> resource
) {
    return std::async(std::launch::deferred, [resource]()
    { return resource; });
}

template<typename T>
std::future<std::shared_ptr<T>> ResourceManager::wrapFuture(
    std::shared_future<std::shared_ptr<void>> f
) {
    return std::async(std::launch::deferred, [f]()
    { return std::static_pointer_cast<T>(f.get()); });
}
