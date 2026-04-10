#ifndef sync_loader_h
#define sync_loader_h

#include "ResourceLoader.h"

class SyncLoader
{
public:
    template<typename T>
    std::shared_ptr<T> load(ResourceLoader<T>& loader, const std::string& path)
    { return loader.load(path); }
};

#endif // sync_loader_h
