#include "../src/engine/include/ResourceLoader/ResourceLoader.h"

#include "../src/engine/include/ResourceLoader/AsyncLoader.h"
#include "../src/engine/include/ResourceLoader/SyncLoader.h"
#include "../src/engine/include/ThreadPool/ThreadPool.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <string>

class ResourceLoaderFixture
{
public:
    struct FakeResource
    {
        int value = 0;
    };

    struct StubResourceLoader : ResourceLoader<FakeResource>
    {
        int calls = 0;
        std::string lastPath;
        bool shouldThrow = false;
        std::shared_ptr<FakeResource> resourceToReturn = std::make_shared<FakeResource>(FakeResource{ 42 });

        std::shared_ptr<FakeResource> load(const std::string& path) override
        {
            this->calls++;
            this->lastPath = path;

            if (this->shouldThrow) throw std::runtime_error("load failed");

            return this->resourceToReturn;
        }
    };
};

TEST_CASE_METHOD(ResourceLoaderFixture, "SyncLoader delegates load and returns loader resource",
    "[unit][resource_loader]"
) {
    StubResourceLoader loader;
    SyncLoader sync;

    const auto result = sync.load(loader, "assets/data/config.json");

    REQUIRE(loader.calls == 1);
    REQUIRE(loader.lastPath == "assets/data/config.json");
    REQUIRE(result == loader.resourceToReturn);
}

TEST_CASE_METHOD(ResourceLoaderFixture, "AsyncLoader resolves resource and forwards path",
    "[unit][resource_loader]"
) {
    StubResourceLoader loader;
    ThreadPool pool(2);
    AsyncLoader async(pool);

    auto future = async.load(loader, "assets/data/config.json");
    const auto result = future.get();

    REQUIRE(loader.calls == 1);
    REQUIRE(loader.lastPath == "assets/data/config.json");
    REQUIRE(result == loader.resourceToReturn);
}

TEST_CASE_METHOD(ResourceLoaderFixture, "AsyncLoader propagates loader exception on get",
    "[unit][resource_loader]"
) {
    StubResourceLoader loader;
    loader.shouldThrow = true;
    ThreadPool pool(2);
    AsyncLoader async(pool);

    auto future = async.load(loader, "assets/data/config.json");

    REQUIRE_THROWS_AS(future.get(), std::runtime_error);
    REQUIRE(loader.calls == 1);
}
