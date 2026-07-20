#ifndef stub_system_initializer_h
#define stub_system_initializer_h

#include "engine/include/ISystemInitializer/ISystemInitializer.h"

class StubSystemInitializer : public ISystemInitializer
{
public:
    void initialize() override {}
    void shutdown() override {}
};

#endif // stub_system_initializer_h
