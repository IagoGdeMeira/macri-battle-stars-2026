#ifndef stub_hurtbox_loader_h
#define stub_hurtbox_loader_h

#include "game/include/EntityFactory/EntityFactory.h"
#include "game/include/HurtboxLoader/HurtboxLoader.h"

class StubHurtboxLoader : public HurtboxLoader
{
public:
    StubHurtboxLoader(DataParser& parser, EntityFactory& factory) : HurtboxLoader(parser, factory) {}

    ControllerMap load(const DataNode& root, Entity parent, bool facingLeft) const override
    { (void)root; (void)parent; (void)facingLeft; return ControllerMap{}; }
};

#endif // stub_hurtbox_loader_h
