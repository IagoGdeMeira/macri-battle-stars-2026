#include "engine/include/Scene/Scene.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/include/SystemManager/SystemManager.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Scene updates systems", "[unit][scene]")
{
    struct TestSystem : System
    {
        bool called = false;
        void update(UpdateContext&) override { this->called = true; }
    };

    EventBus bus;
    Scene scene(bus);

    auto& sys = scene.addSystem<TestSystem>();

    scene.update(0.016f);

    REQUIRE(sys.called);
}

TEST_CASE("Scene passes deltaTime to systems", "[unit][scene]")
{
    struct TestSystem : System
    {
        float received = 0.f;
        void update(UpdateContext& ctx) override { this->received = ctx.deltaTime; }
    };

    EventBus bus;
    Scene scene(bus);

    auto& sys = scene.addSystem<TestSystem>();

    scene.update(0.5f);

    REQUIRE(sys.received == 0.5f);
}

TEST_CASE("Scene flushes CommandBuffer after systems", "[unit][scene]")
{
    struct TestSystem : System
    {
        bool& executed;

        TestSystem(bool& executed) : executed(executed) {}

        void update(UpdateContext& ctx) override
        { ctx.commandBuffer.create([&](World&) { this->executed = true; }); }
    };

    EventBus bus;
    Scene scene(bus);

    bool executed = false;

    scene.addSystem<TestSystem>(executed);

    scene.update(0.016f);

    REQUIRE(executed);
}

TEST_CASE("CommandBuffer executes after system update", "[unit][scene]")
{
    std::vector<int> order;

    struct TestSystem : System
    {
        std::vector<int>& order;

        TestSystem(std::vector<int>& order) : order(order) {}

        void update(UpdateContext& ctx) override
        {
            this->order.push_back(1);
            ctx.commandBuffer.create([&](World&){ this->order.push_back(2); });
        }
    };

    EventBus bus;
    Scene scene(bus);

    scene.addSystem<TestSystem>(order);

    scene.update(0.016f);

    REQUIRE(order.size() == 2);
    REQUIRE(order[0] == 1);
    REQUIRE(order[1] == 2);
}

TEST_CASE("Scene shares World between systems and commands", "[unit][scene]")
{
    struct TestSystem : System
    {
        void update(UpdateContext& ctx) override
        {
            auto e = ctx.world.entities().create();
            ctx.commandBuffer.create([e](World& w) { REQUIRE(w.entities().isAlive(e)); });
        }
    };

    EventBus bus;
    Scene scene(bus);

    scene.addSystem<TestSystem>();

    REQUIRE_NOTHROW(scene.update(0.016f));
}

TEST_CASE("Scene updates multiple systems", "[unit][scene]")
{
    struct TestSystem : System
    {
        int& counter;

        TestSystem(int& counter) : counter(counter) {}

        void update(UpdateContext&) override { this->counter++; }
    };

    EventBus bus;
    Scene scene(bus);

    int counter = 0;

    scene.addSystem<TestSystem>(counter);
    scene.addSystem<TestSystem>(counter);

    scene.update(0.016f);

    REQUIRE(counter == 2);
}
