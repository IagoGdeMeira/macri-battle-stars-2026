#include "engine/include/ResourceLoader/ResourceLoader.h"

#include "StubResourceLoader.h"

#include "engine/include/ResourceLoader/AsyncLoader.h"
#include "engine/include/ResourceLoader/SyncLoader.h"
#include "engine/include/ThreadPool/ThreadPool.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("SyncLoader delegates load and returns loader resource", "[unit][resource_loader]")
{
    StubResourceLoader loader;
    SyncLoader sync;

    const auto result = sync.load(loader, "assets/data/config.json");

    REQUIRE(loader.calls == 1);
    REQUIRE(loader.lastPath == "assets/data/config.json");
    REQUIRE(*result == 42);
}

TEST_CASE("AsyncLoader resolves resource and forwards path", "[unit][resource_loader]")
{
    StubResourceLoader loader;
    ThreadPool pool(2);
    AsyncLoader async(pool);

    auto future = async.load(loader, "assets/data/config.json");
    const auto result = future.get();

    REQUIRE(loader.calls == 1);
    REQUIRE(loader.lastPath == "assets/data/config.json");
    REQUIRE(*result == 42);
}

TEST_CASE("AsyncLoader propagates loader exception on get", "[unit][resource_loader]")
{
    StubResourceLoader loader;
    loader.shouldThrow = true;
    ThreadPool pool(2);
    AsyncLoader async(pool);

    auto future = async.load(loader, "assets/data/config.json");
    REQUIRE_THROWS_AS(future.get(), std::runtime_error);
    REQUIRE(loader.calls == 1);
}
