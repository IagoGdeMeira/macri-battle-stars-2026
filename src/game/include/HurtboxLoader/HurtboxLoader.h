#ifndef hurtbox_loader_h
#define hurtbox_loader_h

#include "domain/components/HurtboxControllerComponent.h"
#include "domain/include/Entity/Entity.h"

#include "engine/include/DataParser/DataParser.h"

class EntityFactory;

class HurtboxLoader
{
public:
    HurtboxLoader(DataParser& parser, EntityFactory& factory) : parser(parser), factory(factory) {}
    HurtboxControllerComponent load(const std::string& path, Entity parent, bool facingLeft);

private:
    DataParser& parser;
    EntityFactory& factory;

    Entity createHurtboxFromNode(const DataNode& node, Entity parent, bool facingLeft);
};

#endif // hurtbox_loader_h
