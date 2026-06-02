#include "../src/engine/include/SystemManager/SystemManager.h"

#include "../src/domain/include/World/World.h"

#include "../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../src/engine/include/EventBus/EventBus.h"
#include "../src/engine/include/System/System.h"
#include "../src/engine/include/UpdateContext/UpdateContext.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("SystemManager addSystem returns valid reference",
    "[unit][system_manager]"
) {
    struct TestSystem : System
    {
        bool alive = true;

        void update(UpdateContext&) override {}
    };

    SystemManager manager;

    auto& sys = manager.addSystem<TestSystem>();

    REQUIRE(sys.alive);
}

TEST_CASE("SystemManager updates systems",
    "[unit][system_manager]"
) {
    struct TestSystem : System
    {
        bool called = false;

        void update(UpdateContext&) override { this->called = true; }
    };

    SystemManager manager;
    auto& sys = manager.addSystem<TestSystem>();

    World world;
    EventBus bus;
    CommandBuffer commands;

    UpdateContext ctx { world, bus, commands, 0.016f };

    manager.update(ctx);

    REQUIRE(sys.called);
}

TEST_CASE("SystemManager preserves execution order",
    "[unit][system_manager]"
) {
    struct TestSystem : System
    {
        std::vector<int>& order;
        int id;

        TestSystem(std::vector<int>& order, int id) : order(order), id(id) {}

        void update(UpdateContext&) override { this->order.push_back(id); }
    };

    SystemManager manager;

    std::vector<int> order;

    manager.addSystem<TestSystem>(order, 1);
    manager.addSystem<TestSystem>(order, 2);
    manager.addSystem<TestSystem>(order, 3);

    World world;
    EventBus bus;
    CommandBuffer commands;

    UpdateContext ctx { world, bus, commands, 0.016f };

    manager.update(ctx);

    REQUIRE(order.size() == 3);
    REQUIRE(order[0] == 1);
    REQUIRE(order[1] == 2);
    REQUIRE(order[2] == 3);
}

TEST_CASE("SystemManager updates multiple systems",
    "[unit][system_manager]"
) {
    struct TestSystem : System
    {
        int& counter;

        TestSystem(int& counter) : counter(counter) {}

        void update(UpdateContext&) override { this->counter++; }
    };

    SystemManager manager;

    int counter = 0;

    manager.addSystem<TestSystem>(counter);
    manager.addSystem<TestSystem>(counter);

    World world;
    EventBus bus;
    CommandBuffer commands;

    UpdateContext ctx { world, bus, commands, 0.016f };

    manager.update(ctx);

    REQUIRE(counter == 2);
}

TEST_CASE("SystemManager passes UpdateContext correctly",
    "[unit][system_manager]"
) {
    struct TestSystem : System
    {
        World* world = nullptr;
        EventBus* bus = nullptr;
        CommandBuffer* commands = nullptr;
        float dt = 0.f;

        void update(UpdateContext& ctx) override
        {
            this->world = &ctx.world;
            this->bus = &ctx.eventBus;
            this->commands = &ctx.commandBuffer;
            this->dt = ctx.deltaTime;
        }
    };

    SystemManager manager;
    auto& sys = manager.addSystem<TestSystem>();

    World world;
    EventBus bus;
    CommandBuffer commands;

    UpdateContext ctx { world, bus, commands, 0.5f };

    manager.update(ctx);

    REQUIRE(sys.world == &world);
    REQUIRE(sys.bus == &bus);
    REQUIRE(sys.commands == &commands);
    REQUIRE(sys.dt == 0.5f);
}

TEST_CASE("SystemManager update with no systems does nothing",
    "[unit][system_manager]"
) {
    SystemManager manager;

    World world;
    EventBus bus;
    CommandBuffer commands;

    UpdateContext ctx { world, bus, commands, 0.016f };

    REQUIRE_NOTHROW(manager.update(ctx));
}
