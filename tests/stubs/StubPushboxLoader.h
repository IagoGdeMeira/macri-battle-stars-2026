#ifndef stub_pushbox_loader_h
#define stub_pushbox_loader_h

#include "game/include/EntityFactory/EntityFactory.h"
#include "game/include/PushboxLoader/PushboxLoader.h"

class StubPushboxLoader : public PushboxLoader
{
public:
    StubPushboxLoader(DataParser& parser, EntityFactory& factory) : PushboxLoader(parser, factory) {}

    ControllerMap load(const DataNode& root, Entity parent, bool facingLeft) const override
    { (void)root; (void)parent; (void)facingLeft; return ControllerMap{}; }
};

#endif // stub_pushbox_loader_h
