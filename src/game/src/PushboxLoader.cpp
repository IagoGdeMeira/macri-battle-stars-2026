#include "PushboxLoader/PushboxLoader.h"

#include "EntityFactory/EntityFactory.h"

#include "domain/components/ActiveComponent.h"
#include "domain/include/World/World.h"
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

        for (auto& hbNode : frameNode->getArray("pushboxes"))
        {
            Entity pushbox = this->createPushboxFromNode(*hbNode, parent);
            frame.pushboxes.push_back(pushbox);
        }
        controller.frames.push_back(std::move(frame));
    }
    return controller;
}

Entity PushboxLoader::createStaticPushbox(const DataNode& node, Entity parent) const
{
    Entity e = this->createPushboxFromNode(node, parent);
    auto& comp = this->factory.world().components();
    comp.get<ActiveComponent>(e).active = true;
    return e;
}

Entity PushboxLoader::createPushboxFromNode(const DataNode& node, Entity parent) const
{
    auto posNode = node.has("position") ? node.getObject("position") : nullptr;
    Position offset = posNode ? DataUtils::parsePosition(*posNode) : Position{0.f, 0.f};

    PushboxComponent::Type pushType = this->parsePushboxType(node.getString("pushboxType", "dynamic"));
    float mass = node.getFloat("mass", 1.f);
    float pushResistance = node.getFloat("pushResistance", 1.f);
    DebugConfig debug = DataUtils::parseDebug(node, {false, Color{0, 0, 255, 255}});

    std::string shape = node.getString("shape", "rectangle");
    if (shape == "rectangle") return this->factory.createPushboxChild(EntityFactory::PushboxChildParams{
        parent, offset, pushType, mass, pushResistance, debug}, DataUtils::parseRect(node));
    if (shape == "circle") return this->factory.createPushboxChild(EntityFactory::PushboxChildParams{
        parent, offset, pushType, mass, pushResistance, debug}, DataUtils::parseCircle(node));
    throw std::runtime_error("Invalid pushbox shape: " + shape);
}

PushboxComponent::Type PushboxLoader::parsePushboxType(const std::string& typeStr) const
{
    if (typeStr == "static")    return PushboxComponent::Type::Static;
    if (typeStr == "dynamic")   return PushboxComponent::Type::Dynamic;
    throw std::runtime_error("Invalid pushbox type: " + typeStr);
}
