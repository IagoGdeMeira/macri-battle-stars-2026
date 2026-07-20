#ifndef async_loader_h
#define async_loader_h

#include "ResourceLoader.h"
#include "ThreadPool/ThreadPool.h"

class AsyncLoader
{
public:
    AsyncLoader(ThreadPool& pool) : pool(pool) {}

    template<typename T>
    std::future<std::shared_ptr<T>> load(ResourceLoader<T>& loader, const std::string& path)
    { return this->pool.enqueue([&loader, path]() { return loader.load(path); }); }

private:
    ThreadPool& pool;
};

#endif // async_loader_h
