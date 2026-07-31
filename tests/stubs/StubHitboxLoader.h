#ifndef stub_hitbox_loader_h
#define stub_hitbox_loader_h

#include "game/include/EntityFactory/EntityFactory.h"
#include "game/include/HitboxLoader/HitboxLoader.h"

class StubHitboxLoader : public HitboxLoader
{
public:
    StubHitboxLoader(DataParser& parser, EntityFactory& factory) : HitboxLoader(parser, factory) {}

    ControllerMap load(const DataNode& root, Entity parent) const override
    { (void)root; (void)parent; return ControllerMap{}; }
};

#endif // stub_hitbox_loader_h
