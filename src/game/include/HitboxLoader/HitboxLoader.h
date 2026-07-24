#ifndef hitbox_loader_h
#define hitbox_loader_h

#include "domain/components/HitboxControllerComponent.h"

#include "engine/include/DataParser/DataParser.h"

class EntityFactory;

class HitboxLoader
{
public:
    HitboxLoader(DataParser& parser, EntityFactory& factory) : parser(parser), factory(factory) {}
    HitboxControllerComponent load(const std::string& path, Entity parent, bool facingLeft);

private:
    DataParser& parser;
    EntityFactory& factory;
};

#endif // hitbox_loader_h
