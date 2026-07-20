#include "engine/include/InputManager/InputManager.h"

#include "StubInputAdapter.h"
#include "StubPlatformEventProvider.h"

#include "engine/events/PlatformEvent.h"
#include "engine/value_objects/KeyCode/KeyCode.h"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <memory>

TEST_CASE("InputManager dispatches provider events to all adapters", "[unit][input_manager]")
{
    InputManager manager;

    auto provider = std::make_unique<StubPlatformEventProvider>();
    provider->platformEvents.push_back(std::make_unique<KeyboardEvent>(static_cast<uint32_t>(KeyCode::A), true));

    manager.setProvider(std::move(provider));

    int firstCalls = 0, secondCalls = 0;
    auto firstAdapter = std::make_unique<StubInputAdapter>(firstCalls);
    auto secondAdapter = std::make_unique<StubInputAdapter>(secondCalls);

    manager.addAdapter(std::move(firstAdapter));
    manager.addAdapter(std::move(secondAdapter));

    manager.poll();

    REQUIRE(firstCalls == 1);
    REQUIRE(secondCalls == 1);
}

TEST_CASE("InputManager ignores polling without a provider", "[unit][input_manager]")
{
    InputManager manager;

    int adapterCalls = 0;
    auto adapter = std::make_unique<StubInputAdapter>(adapterCalls);

    manager.addAdapter(std::move(adapter));
    manager.poll();

    REQUIRE(adapterCalls == 0);
}
