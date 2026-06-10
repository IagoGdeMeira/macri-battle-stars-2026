#ifndef stub_loader_h
#define stub_loader_h

#include "../../src/engine/include/ResourceLoader/ResourceLoader.h"

#include <future>
#include <memory>
#include <string>

class StubLoader : public ResourceLoader<int>
{
public:
    int loadCalls = 0;
    std::string lastPath;
    std::shared_ptr<int> resourceToReturn = std::make_shared<int>(42);
    std::shared_ptr<std::promise<void>> gate = std::make_shared<std::promise<void>>();
    std::shared_future<void> gateFuture = gate->get_future().share();
    bool blockUntilReleased = false;

    std::shared_ptr<int> load(const std::string& path) override
    {
        this->loadCalls++;
        this->lastPath = path;
        if (this->blockUntilReleased) this->gateFuture.wait();
        return this->resourceToReturn;
    }
};

#endif // stub_loader_h
