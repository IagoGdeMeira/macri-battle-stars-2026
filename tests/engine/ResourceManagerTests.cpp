#include "../../src/engine/include/ResourceManager/ResourceManager.h"

#include "../stubs/StubLoader.h"

#include "../../src/engine/include/ResourceLoader/ResourceLoader.h"
#include "../../src/engine/include/ThreadPool/ThreadPool.h"

#include <catch2/catch_test_macros.hpp>
#include <future>
#include <memory>
#include <string>

TEST_CASE("ResourceManager caches synchronous loads", "[unit][resource_manager]")
{
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

TEST_CASE("ResourceManager returns cached resource through async load after sync load", "[unit][resource_manager]")
{
    ThreadPool pool(2);
    ResourceManager manager(pool);
    StubLoader loader;

    const auto cached = manager.load(loader, "assets/data/config.json");
    auto future = manager.loadAsync(loader, "assets/data/config.json");

    REQUIRE(loader.loadCalls == 1);
    REQUIRE(future.get() == cached);
}

TEST_CASE("ResourceManager shares pending async loads and caches the resolved resource", "[unit][resource_manager]")
{
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

TEST_CASE("ResourceManager reloads resource when weak cache entry expires", "[unit][resource_manager]")
{
    ThreadPool pool(2);
    ResourceManager manager(pool);
    StubLoader loader;

    {
        auto firstResource = std::make_shared<int>(1);
        loader.resourceToReturn = firstResource;
        auto first = manager.load(loader, "assets/data/config.json");
        REQUIRE(first == firstResource);
    }

    auto secondResource = std::make_shared<int>(2);
    loader.resourceToReturn = secondResource;
    auto second = manager.load(loader, "assets/data/config.json");

    REQUIRE(loader.loadCalls == 2);
    REQUIRE(second == secondResource);
    REQUIRE(*second == 2);
}

TEST_CASE("ResourceManager reuses async-resolved resource in subsequent sync load", "[unit][resource_manager]")
{
    ThreadPool pool(2);
    ResourceManager manager(pool);
    StubLoader loader;

    auto asyncFuture = manager.loadAsync(loader, "assets/data/config.json");
    auto asyncResult = asyncFuture.get();
    auto syncResult = manager.load(loader, "assets/data/config.json");

    REQUIRE(loader.loadCalls == 1);
    REQUIRE(syncResult == asyncResult);
}
