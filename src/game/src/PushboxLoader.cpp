#include "PushboxLoader/PushboxLoader.h"

#include "EntityFactory/EntityFactory.h"

#include "domain/utils/Logger/Logger.h"
#include "domain/value_objects/StateId/StateId.h"

#include "engine/utils/DataUtils/DataUtils.h"
#include "engine/value_objects/DebugConfig/DebugConfig.h"

#include <stdexcept>

PushboxControllerComponent PushboxLoader::loadSingleState(const DataNode& stateNode, Entity parent) const
{
    PushboxControllerComponent controller;
    controller.loop = stateNode.getBool("loop", false);

    for (auto& frameNode : stateNode.getArray("frames"))
    {
        PushboxControllerComponent::Frame frame;
        frame.duration = frameNode->getFloat("duration", 0.f);

        for (auto& hbNode : frameNode->getArray("hitboxes"))
        {
            Entity pushbox = this->createPushboxFromNode(*hbNode, parent);
            frame.pushboxes.push_back(pushbox);
        }
        controller.frames.push_back(std::move(frame));
    }
    return controller;
}

Entity PushboxLoader::createPushboxFromNode(const DataNode& node, Entity parent) const
{
    LOG_DEBUG("PushboxLoader: node has 'position'={}, has 'size'={}, has 'debug'={}",
        node.has("position"), node.has("size"), node.has("debug"));
    LOG_DEBUG("PushboxLoader: node has 'x'={}, has 'y'={}, has 'width'={}, has 'height'={}",
        node.has("x"), node.has("y"), node.has("width"), node.has("height"));

    auto posNode = node.has("position") ? node.getObject("position") : nullptr;
    Position offset = posNode ? DataUtils::parsePosition(*posNode) : Position{0.f, 0.f};

    PushboxComponent::Type pushType = this->parsePushboxType(node.getString("pushboxType", "dynamic"));
    float mass = node.getFloat("mass", 1.f);
    float pushResistance = node.getFloat("pushResistance", 1.f);
    DebugConfig debug = DataUtils::parseDebug(node, {false, Color{0, 0, 255, 255}});

    std::string type = node.getString("type", "rectangle");
    if (type == "rectangle")
    {
        Rectangle rect = DataUtils::parseRect(node);
        LOG_DEBUG("PushboxLoader: parsed rect size=({},{}) offset=({},{}) debug.color=({},{},{},{})",
            rect.size.width, rect.size.height, offset.x, offset.y,
           static_cast<int>(debug.color.r), static_cast<int>(debug.color.g),
           static_cast<int>(debug.color.b), static_cast<int>(debug.color.a));
        return this->factory.createPushboxChild(EntityFactory::PushboxChildParams{
            parent, offset, pushType, mass, pushResistance, debug}, rect);
    }
    if (type == "circle")
    {
        Circle circle = DataUtils::parseCircle(node);
        LOG_DEBUG("PushboxLoader: parsed circle radius={} offset=({},{})", circle.radius, offset.x, offset.y);
        return this->factory.createPushboxChild(EntityFactory::PushboxChildParams{
            parent, offset, pushType, mass, pushResistance, debug}, circle);
    }

    throw std::runtime_error("Invalid pushbox type: " + type);
}

PushboxComponent::Type PushboxLoader::parsePushboxType(const std::string& typeStr) const
{
    if (typeStr == "static")    return PushboxComponent::Type::Static;
    if (typeStr == "dynamic")   return PushboxComponent::Type::Dynamic;
    throw std::runtime_error("Invalid pushbox type: " + typeStr);
}
