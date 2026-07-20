#ifndef stub_resource_loader_h
#define stub_resource_loader_h

#include "engine/include/ResourceLoader/ResourceLoader.h"

#include <memory>
#include <string>
#include <stdexcept>

class StubResourceLoader : public ResourceLoader<int>
{
public:
    int calls = 0;
    std::string lastPath;
    bool shouldThrow = false;
    std::shared_ptr<int> resourceToReturn = std::make_shared<int>(42);

    std::shared_ptr<int> load(const std::string& path) override
    {
        this->calls++;
        this->lastPath = path;
        if (this->shouldThrow) throw std::runtime_error("load failed");
        return this->resourceToReturn;
    }
};

#endif // stub_resource_loader_h
