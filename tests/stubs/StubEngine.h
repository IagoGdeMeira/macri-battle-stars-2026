#ifndef stub_engine_h
#define stub_engine_h

#include "engine/include/Engine/Engine.h"

class StubEngine : public Engine
{
public:
    StubEngine(Window& w, GameSettings& s) : Engine(w, s) {}
};

#endif // stub_engine_h
