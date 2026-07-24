#ifndef pushbox_loader_h
#define pushbox_loader_h

#include "domain/components/PushboxControllerComponent.h"
#include "domain/include/Entity/Entity.h"

#include "engine/include/DataParser/DataParser.h"

class EntityFactory;

class PushboxLoader
{
public:
    PushboxLoader(DataParser& parser, EntityFactory& factory) : parser(parser), factory(factory) {}
    PushboxControllerComponent load(const std::string& path, Entity parent, bool facingLeft);

private:
    DataParser& parser;
    EntityFactory& factory;

    Entity createPushboxFromNode(const DataNode& node, Entity parent, bool facingLeft);

    PushboxComponent::Type parsePushboxType(const std::string& typeStr);
};

#endif // pushbox_loader_h
