#ifndef pushbox_loader_h
#define pushbox_loader_h

#include "domain/components/PushboxComponent.h"
#include "domain/components/PushboxControllerComponent.h"
#include "domain/value_objects/StateId/StateId.h"

#include "engine/include/DataNode/DataNode.h"
#include "engine/include/DataParser/DataParser.h"

#include <unordered_map>

class EntityFactory;

class PushboxLoader
{
public:
    PushboxLoader(DataParser& parser, EntityFactory& factory) : parser(parser), factory(factory) {}

    using ControllerMap = std::unordered_map<StateId, PushboxControllerComponent, StateId::Hash>;
    virtual ControllerMap load(const DataNode& root, Entity parent) const;

private:
    DataParser& parser;
    EntityFactory& factory;

    Entity createPushboxFromNode(const DataNode& node, Entity parent) const;
    PushboxComponent::Type parsePushboxType(const std::string& typeStr) const;
};

#endif // pushbox_loader_h
