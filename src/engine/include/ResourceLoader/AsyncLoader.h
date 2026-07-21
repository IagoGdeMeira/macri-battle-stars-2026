#ifndef async_loader_h
#define async_loader_h

#include "ResourceLoader.h"
#include "ThreadPool/ThreadPool.h"

#include <functional>

class AsyncLoader
{
public:
    AsyncLoader(ThreadPool& pool) : pool(pool) {}

    template<typename T>
    std::future<std::shared_ptr<T>> load(ResourceLoader<T>& loader, const std::string& path)
    { return this->pool.enqueue([&loader, path]() { return loader.load(path); }); }

    void enqueueTask(std::function<void()> task) { this->pool.enqueue([task]() { task(); }); }

private:
    ThreadPool& pool;
};

#endif // async_loader_h
