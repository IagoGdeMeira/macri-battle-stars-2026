#include "HurtboxLoader/HurtboxLoader.h"

#include "EntityFactory/EntityFactory.h"

#include "domain/value_objects/StateId/StateId.h"

#include "engine/utils/DataUtils/DataUtils.h"
#include "engine/value_objects/DebugConfig/DebugConfig.h"

#include <stdexcept>

HurtboxControllerComponent HurtboxLoader::loadSingleState(const DataNode& stateNode, Entity parent) const
{
    HurtboxControllerComponent controller;
    controller.frameDuration = stateNode.getFloat("frameDuration", 0.1f);
    controller.loop = stateNode.getBool("loop", false);

    for (auto& frameNode : stateNode.getArray("frames"))
    {
        HurtboxControllerComponent::Frame frame;
        frame.duration = frameNode->getFloat("duration", 0.f);
        if (frame.duration == 0.f) frame.duration = controller.frameDuration;

        for (auto& hbNode : frameNode->getArray("hurtboxes"))
        {
            Entity hurtbox = this->createHurtboxFromNode(*hbNode, parent);
            frame.hurtboxes.push_back(hurtbox);
        }
        controller.frames.push_back(std::move(frame));
    }
    return controller;
}

Entity HurtboxLoader::createHurtboxFromNode(const DataNode& node, Entity parent) const
{
    auto posNode = node.has("position") ? node.getObject("position") : nullptr;
    Position offset = posNode ? DataUtils::parsePosition(*posNode) : Position{0.f, 0.f};

    float damageMultiplier = node.getFloat("damageMultiplier", 0.f);
    DebugConfig debug = DataUtils::parseDebug(node, {false, Color{0, 255, 0, 255}});

    std::string shape = node.getString("shape", "rectangle");
    if (shape == "rectangle") return this->factory.createHurtboxChild(EntityFactory::HurtboxChildParams{
        parent, offset, damageMultiplier, debug}, DataUtils::parseRect(node));
    if (shape == "circle") return this->factory.createHurtboxChild(EntityFactory::HurtboxChildParams{
        parent, offset, damageMultiplier, debug}, DataUtils::parseCircle(node));
    throw std::runtime_error("Invalid hurtbox shape: " + shape);
}
