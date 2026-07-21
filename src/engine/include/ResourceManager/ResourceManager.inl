#ifndef resource_manager_inl
#define resource_manager_inl

#include "ResourceManager.h"

#include <typeinfo>

template<typename T>
std::shared_ptr<T> ResourceManager::load(ResourceLoader<T>& loader, const std::string& path)
{
    auto key = this->makeKey<T>(path);

    auto it = this->resources.find(key);
    if (it != this->resources.end()) if (auto existing = it->second.lock())
    { return std::static_pointer_cast<T>(existing); }
    
    auto resource = this->sync.load(loader, path);
    this->resources[key] = resource;

    return resource;
}

template<typename T>
std::future<std::shared_ptr<T>> ResourceManager::loadAsync(ResourceLoader<T>& loader, const std::string& path)
{
    auto key = this->makeKey<T>(path);

    {
        std::lock_guard<std::mutex> lock(this->mutex);

        auto it = this->resources.find(key);
        if (it != this->resources.end()) if (auto existing = it->second.lock())
        { return this->makeReadyFuture(std::static_pointer_cast<T>(existing)); }

        auto itLoading = this->loading.find(key);
        if (itLoading != this->loading.end()) return this->wrapFuture<T>(itLoading->second);
    }

    auto task = std::make_shared<std::packaged_task<std::shared_ptr<T>()>>(
        [&loader, path]() { return loader.load(path); });
    auto future = task->get_future();
    auto sharedFuture = future.share();

    auto erasedFuture = std::shared_future<std::shared_ptr<void>>(
        std::async(std::launch::deferred, [sharedFuture]()
        { return std::static_pointer_cast<void>(sharedFuture.get()); }));

    {
        std::lock_guard<std::mutex> lock(this->mutex);
        this->loading[key] = erasedFuture;
    }

    auto currentVersion = this->version.load();

    this->async.enqueueTask([task]() { (*task)(); });

    return std::async(std::launch::deferred, [this, key, sharedFuture, currentVersion]()
    {
        auto resource = sharedFuture.get();
        {
            std::lock_guard<std::mutex> lock(this->mutex);
            if (this->version.load() == currentVersion) this->resources[key] = resource;
            this->loading.erase(key);
        }
        return resource;
    });
}

template<typename T>
std::string ResourceManager::makeKey(const std::string& path)
{ return std::string(typeid(T).name()) + ":" + path; }

template<typename T>
std::future<std::shared_ptr<T>> ResourceManager::makeReadyFuture(std::shared_ptr<T> resource)
{ return std::async(std::launch::deferred, [resource]() { return resource; }); }

template<typename T>
std::future<std::shared_ptr<T>> ResourceManager::wrapFuture(std::shared_future<std::shared_ptr<void>> future)
{ return std::async(std::launch::deferred, [future]() { return std::static_pointer_cast<T>(future.get()); }); }

#endif // resource_manager_inl
