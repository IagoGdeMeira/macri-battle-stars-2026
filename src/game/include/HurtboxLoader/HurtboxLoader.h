#ifndef hurtbox_loader_h
#define hurtbox_loader_h

#include "domain/components/HurtboxControllerComponent.h"
#include "domain/value_objects/StateId/StateId.h"

#include "engine/include/DataNode/DataNode.h"
#include "engine/include/DataParser/DataParser.h"

#include <unordered_map>

class EntityFactory;

class HurtboxLoader
{
public:
    HurtboxLoader(DataParser& parser, EntityFactory& factory) : parser(parser), factory(factory) {}
    virtual HurtboxControllerComponent loadSingleState(const DataNode& stateNode, Entity parent) const;

private:
    DataParser& parser;
    EntityFactory& factory;

    Entity createHurtboxFromNode(const DataNode& node, Entity parent) const;
};

#endif // hurtbox_loader_h
