#ifndef stub_hitbox_loader_h
#define stub_hitbox_loader_h

#include "game/include/EntityFactory/EntityFactory.h"
#include "game/include/HitboxLoader/HitboxLoader.h"

class StubHitboxLoader : public HitboxLoader
{
public:
    StubHitboxLoader(DataParser& parser, EntityFactory& factory) : HitboxLoader(parser, factory) {}

    ControllerMap load(const DataNode& root, Entity parent, bool facingLeft) const override
    { (void)root; (void)parent; (void)facingLeft; return ControllerMap{}; }
};

#endif // stub_hitbox_loader_h
