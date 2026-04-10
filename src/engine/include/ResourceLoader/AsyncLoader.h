#ifndef async_loader_h
#define async_loader_h

#include "ResourceLoader.h"

#include <future>

class AsyncLoader
{
public:
    template<typename T>
    std::future<std::shared_ptr<T>> load(ResourceLoader<T>& loader, const std::string& path)
    { return std::async(std::launch::async, [&loader, path]() { return loader.load(path); }); }
};

#endif // async_loader_h
