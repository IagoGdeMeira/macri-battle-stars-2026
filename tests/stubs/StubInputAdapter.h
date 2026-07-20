#ifndef stub_input_adapter_h
#define stub_input_adapter_h

#include "engine/include/InputAdapter/InputAdapter.h"

class StubInputAdapter : public InputAdapter
{
public:
    int lastEventCount = 0;

    explicit StubInputAdapter(int& counter) : callCounter(counter) {}

    StubInputAdapter() : callCounter(this->dummy) {}

    void processEvents(const std::vector<std::unique_ptr<PlatformEvent>>& events) override
    {
        ++this->callCounter;
        this->lastEventCount = static_cast<int>(events.size());
    }

    int getCallCount() const { return this->callCounter; }

private:
    int& callCounter;
    int dummy = 0;
};

#endif // stub_input_adapter_h
