#include "PushboxLoader/PushboxLoader.h"

#include "EntityFactory/EntityFactory.h"

#include "domain/components/PushboxComponent.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include "engine/include/DataUtils/DataUtils.h"

#include <stdexcept>

PushboxControllerComponent PushboxLoader::load(const std::string& path, Entity parent, bool facingLeft)
{
    auto root = this->parser.parse(path);
    PushboxControllerComponent controller;
    controller.loop = root->getBool("loop", false);

    for (auto& frameNode : root->getArray("frames"))
    {
        PushboxControllerComponent::Frame frame;
        frame.duration = frameNode->getFloat("duration", 0.f);

        for (auto& pbNode : frameNode->getArray("pushboxes"))
        {
            Entity pushbox = this->createPushboxFromNode(*pbNode, parent, facingLeft);
            frame.pushboxes.push_back(pushbox);
        }
        controller.frames.push_back(std::move(frame));
    }

    return controller;
}

Entity PushboxLoader::createPushboxFromNode(const DataNode& node, Entity parent, bool facingLeft)
{
    std::string type = node.getString("type", "rectangle");
    Position offset = DataUtils::parsePosition(node, Position{0.f, 0.f});
    PushboxComponent::Type pushType = this->parsePushboxType(node.getString("pushboxType", "dynamic"));
    float mass = node.getFloat("mass", 1.f);
    float pushResistance = node.getFloat("pushResistance", 1.f);

    if (type == "rectangle")
    {
        Rectangle rect = DataUtils::parseRect(node);
        return this->factory.createPushboxChild(EntityFactory::PushboxChildParams{
            parent, offset, pushType, mass, pushResistance, facingLeft}, rect);
    }
    if (type == "circle")
    {
        Circle circle = DataUtils::parseCircle(node);
        return this->factory.createPushboxChild(EntityFactory::PushboxChildParams{
            parent, offset, pushType, mass, pushResistance, facingLeft}, circle);
    }
    throw std::runtime_error("Invalid pushbox type: " + type);
}

PushboxComponent::Type PushboxLoader::parsePushboxType(const std::string& typeStr)
{
    if (typeStr == "static") return PushboxComponent::Type::Static;
    if (typeStr == "dynamic") return PushboxComponent::Type::Dynamic;
    throw std::runtime_error("Invalid pushbox type: " + typeStr);
}
