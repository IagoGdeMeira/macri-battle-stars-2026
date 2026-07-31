#ifndef stub_hurtbox_loader_h
#define stub_hurtbox_loader_h

#include "game/include/EntityFactory/EntityFactory.h"
#include "game/include/HurtboxLoader/HurtboxLoader.h"

class StubHurtboxLoader : public HurtboxLoader
{
public:
    StubHurtboxLoader(DataParser& parser, EntityFactory& factory) : HurtboxLoader(parser, factory) {}

    HurtboxControllerComponent loadSingleState(const DataNode& stateNode, Entity parent) const override
    { (void)stateNode; (void)parent; return HurtboxControllerComponent{}; }
};

#endif // stub_hurtbox_loader_h
