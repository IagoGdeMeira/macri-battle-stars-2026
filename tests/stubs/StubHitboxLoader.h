#ifndef stub_hitbox_loader_h
#define stub_hitbox_loader_h

#include "game/include/EntityFactory/EntityFactory.h"
#include "game/include/HitboxLoader/HitboxLoader.h"

class StubHitboxLoader : public HitboxLoader
{
public:
    StubHitboxLoader(DataParser& parser, EntityFactory& factory) : HitboxLoader(parser, factory) {}
    
    HitboxControllerComponent loadSingleState(const DataNode& stateNode, Entity parent) const override
    { (void)stateNode; (void)parent; return HitboxControllerComponent{}; }
};

#endif // stub_hitbox_loader_h
