#include "../src/engine/include/ResourceManager/ResourceManager.h"

#include "../src/engine/include/ResourceLoader/ResourceLoader.h"
#include "../src/engine/include/ThreadPool/ThreadPool.h"

#include <catch2/catch_test_macros.hpp>

#include <future>
#include <memory>
#include <string>

class ResourceManagerFixture
{
public:
    struct FakeResource { int value = 0; };

    struct StubLoader : ResourceLoader<FakeResource>
    {
        int loadCalls = 0;
        std::string lastPath;
        std::shared_ptr<FakeResource> resourceToReturn = std::make_shared<FakeResource>(FakeResource{ 42 });
        std::shared_ptr<std::promise<void>> gate = std::make_shared<std::promise<void>>();
        std::shared_future<void> gateFuture = gate->get_future().share();
        bool blockUntilReleased = false;

        std::shared_ptr<FakeResource> load(const std::string& path) override
        {
            this->loadCalls++;
            this->lastPath = path;

            if (this->blockUntilReleased) this->gateFuture.wait();
            return this->resourceToReturn;
        }
    };
};

TEST_CASE_METHOD(ResourceManagerFixture, "ResourceManager caches synchronous loads",
    "[unit][resource_manager]"
) {
    ThreadPool pool(2);
    ResourceManager manager(pool);
    StubLoader loader;

    const auto first = manager.load(loader, "assets/data/config.json");
    const auto second = manager.load(loader, "assets/data/config.json");

    REQUIRE(loader.loadCalls == 1);
    REQUIRE(loader.lastPath == "assets/data/config.json");
    REQUIRE(first == loader.resourceToReturn);
    REQUIRE(second == loader.resourceToReturn);
    REQUIRE(first == second);
}

TEST_CASE_METHOD(ResourceManagerFixture, "ResourceManager returns cached resource through async load after sync load",
    "[unit][resource_manager]"
) {
    ThreadPool pool(2);
    ResourceManager manager(pool);
    StubLoader loader;

    const auto cached = manager.load(loader, "assets/data/config.json");
    auto future = manager.loadAsync(loader, "assets/data/config.json");

    REQUIRE(loader.loadCalls == 1);
    REQUIRE(future.get() == cached);
}

TEST_CASE_METHOD(ResourceManagerFixture, "ResourceManager shares pending async loads and caches the resolved resource",
    "[unit][resource_manager]"
) {
    ThreadPool pool(2);
    ResourceManager manager(pool);
    StubLoader loader;
    loader.blockUntilReleased = true;

    auto firstFuture = manager.loadAsync(loader, "assets/data/config.json");
    auto secondFuture = manager.loadAsync(loader, "assets/data/config.json");

    loader.gate->set_value();

    const auto first = firstFuture.get();
    const auto second = secondFuture.get();

    REQUIRE(loader.loadCalls == 1);

    REQUIRE(first == loader.resourceToReturn);
    REQUIRE(second == loader.resourceToReturn);
    REQUIRE(first == second);

    auto thirdFuture = manager.loadAsync(loader, "assets/data/config.json");

    REQUIRE(loader.loadCalls == 1);
    REQUIRE(thirdFuture.get() == loader.resourceToReturn);
}

TEST_CASE_METHOD(ResourceManagerFixture, "ResourceManager reloads resource when weak cache entry expires",
    "[unit][resource_manager]"
) {
    ThreadPool pool(2);
    ResourceManager manager(pool);
    StubLoader loader;

    {
        auto firstResource = std::make_shared<FakeResource>(FakeResource{ 1 });
        loader.resourceToReturn = firstResource;
        auto first = manager.load(loader, "assets/data/config.json");
        REQUIRE(first == firstResource);
    }

    auto secondResource = std::make_shared<FakeResource>(FakeResource{ 2 });
    loader.resourceToReturn = secondResource;
    auto second = manager.load(loader, "assets/data/config.json");

    REQUIRE(loader.loadCalls == 2);
    REQUIRE(second == secondResource);
    REQUIRE(second->value == 2);
}

TEST_CASE_METHOD(ResourceManagerFixture, "ResourceManager reuses async-resolved resource in subsequent sync load",
    "[unit][resource_manager]"
) {
    ThreadPool pool(2);
    ResourceManager manager(pool);
    StubLoader loader;

    auto asyncFuture = manager.loadAsync(loader, "assets/data/config.json");
    auto asyncResult = asyncFuture.get();
    auto syncResult = manager.load(loader, "assets/data/config.json");

    REQUIRE(loader.loadCalls == 1);
    REQUIRE(syncResult == asyncResult);
}
