#ifndef resource_manager_h
#define resource_manager_h

#include "ResourceLoader/AsyncLoader.h"
#include "ResourceLoader/ResourceLoader.h"
#include "ResourceLoader/SyncLoader.h"
#include "ThreadPool/ThreadPool.h"

#include <atomic>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

class ResourceManager
{
public:
    ResourceManager(ThreadPool& pool) : async(pool) {}

    template<typename T>
    std::shared_ptr<T> load(ResourceLoader<T>& loader, const std::string& path);

    template<typename T>
    std::future<std::shared_ptr<T>> loadAsync(ResourceLoader<T>& loader, const std::string& path);

    virtual void clearCache();

private:
    std::atomic<uint64_t> version{0};
    std::unordered_map<std::string, std::weak_ptr<void>> resources;
    std::unordered_map<std::string, std::shared_future<std::shared_ptr<void>>> loading;
    std::mutex mutex;

    SyncLoader sync;
    AsyncLoader async;

    template<typename T>
    std::string makeKey(const std::string& path);

    template<typename T>
    std::future<std::shared_ptr<T>> makeReadyFuture(std::shared_ptr<T> resource);

    template<typename T>
    std::future<std::shared_ptr<T>> wrapFuture(std::shared_future<std::shared_ptr<void>> future);
};

#include "ResourceManager.inl"

#endif // resource_manager_h
