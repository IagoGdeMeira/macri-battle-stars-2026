#ifndef stub_pushbox_loader_h
#define stub_pushbox_loader_h

#include "game/include/EntityFactory/EntityFactory.h"
#include "game/include/PushboxLoader/PushboxLoader.h"

class StubPushboxLoader : public PushboxLoader
{
public:
    StubPushboxLoader(DataParser& parser, EntityFactory& factory) : PushboxLoader(parser, factory) {}

    PushboxControllerComponent loadSingleState(const DataNode& stateNode, Entity parent) const override
    { (void)stateNode; (void)parent; return PushboxControllerComponent{}; }
};

#endif // stub_pushbox_loader_h
