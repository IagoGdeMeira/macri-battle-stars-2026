#ifndef hitbox_loader_h
#define hitbox_loader_h

#include "domain/components/HitboxControllerComponent.h"
#include "domain/value_objects/StateId/StateId.h"

#include "engine/include/DataNode/DataNode.h"
#include "engine/include/DataParser/DataParser.h"

#include <unordered_map>

class EntityFactory;

class HitboxLoader
{
public:
    HitboxLoader(DataParser& parser, EntityFactory& factory) : parser(parser), factory(factory) {}

    using ControllerMap = std::unordered_map<StateId, HitboxControllerComponent, StateId::Hash>;
    virtual ControllerMap load(const DataNode& root, Entity parent, bool facingLeft) const;

private:
    DataParser& parser;
    EntityFactory& factory;

    Entity createHitboxFromNode(const DataNode& node, Entity parent, bool facingLeft) const;
};

#endif // hitbox_loader_h
