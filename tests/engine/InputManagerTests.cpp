#include "../src/engine/include/InputManager/InputManager.h"

#include "../src/engine/events/PlatformEvent.h"
#include "../src/engine/include/IPlatformEventProvider/IPlatformEventProvider.h"
#include "../src/engine/include/InputAdapter/InputAdapter.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <vector>

class InputManagerTestsFixture
{
public:
    class StubProvider : public IPlatformEventProvider
    {
    public:
        explicit StubProvider(int& polls) : polls(polls) {}

        std::vector<std::unique_ptr<PlatformEvent>> pollEvents() override
        {
            ++this->polls;

            std::vector<std::unique_ptr<PlatformEvent>> events;
            events.push_back(std::make_unique<KeyboardEvent>(0, true));
            return events;
        }

    private:
        int& polls;
    };

    class StubAdapter : public InputAdapter
    {
    public:
        explicit StubAdapter(int& calls) : calls(calls) {}

        void processEvents(const std::vector<std::unique_ptr<PlatformEvent>>& events) override
        { ++this->calls; this->lastEventCount = static_cast<int>(events.size()); }

        int lastEventCount = 0;

    private:
        int& calls;
    };
};

TEST_CASE_METHOD(InputManagerTestsFixture, "InputManager dispatches provider events to all adapters",
    "[unit][input_manager]"
) {
    InputManager manager;

    int providerPolls = 0;
    auto provider = std::make_unique<StubProvider>(providerPolls);
    manager.setProvider(std::move(provider));

    int firstCalls = 0;
    int secondCalls = 0;
    auto firstAdapter = std::make_unique<StubAdapter>(firstCalls);
    auto* firstAdapterPtr = firstAdapter.get();
    auto secondAdapter = std::make_unique<StubAdapter>(secondCalls);
    auto* secondAdapterPtr = secondAdapter.get();

    manager.addAdapter(std::move(firstAdapter));
    manager.addAdapter(std::move(secondAdapter));

    manager.poll();

    REQUIRE(providerPolls == 1);
    REQUIRE(firstCalls == 1);
    REQUIRE(secondCalls == 1);
    REQUIRE(firstAdapterPtr->lastEventCount == 1);
    REQUIRE(secondAdapterPtr->lastEventCount == 1);
}

TEST_CASE_METHOD(InputManagerTestsFixture, "InputManager ignores polling without a provider",
    "[unit][input_manager]"
) {
    InputManager manager;

    int adapterCalls = 0;
    auto adapter = std::make_unique<StubAdapter>(adapterCalls);
    auto* adapterPtr = adapter.get();

    manager.addAdapter(std::move(adapter));
    manager.poll();

    REQUIRE(adapterCalls == 0);
    REQUIRE(adapterPtr->lastEventCount == 0);
}
