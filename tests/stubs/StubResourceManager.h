#ifndef stub_resource_manager_h
#define stub_resource_manager_h

#include "engine/include/ResourceManager/ResourceManager.h"
#include "engine/include/ThreadPool/ThreadPool.h"

class StubResourceManager : public ResourceManager
{
public:
    StubResourceManager() : ResourceManager(this->pool) {}

private:
    ThreadPool pool{1};
};

#endif // stub_resource_manager_h
