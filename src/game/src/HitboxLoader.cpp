#include "HitboxLoader/HitboxLoader.h"

#include "EntityFactory/EntityFactory.h"

#include "domain/value_objects/StateId/StateId.h"

#include "engine/utils/DataUtils/DataUtils.h"
#include "engine/value_objects/DebugConfig/DebugConfig.h"

#include <stdexcept>

HitboxControllerComponent HitboxLoader::loadSingleState(const DataNode& stateNode, Entity parent) const
{
    HitboxControllerComponent controller;
    controller.loop = stateNode.getBool("loop", false);

    for (auto& frameNode : stateNode.getArray("frames"))
    {
        HitboxControllerComponent::Frame frame;
        frame.duration = frameNode->getFloat("duration", 0.f);

        for (auto& hbNode : frameNode->getArray("hitboxes"))
        {
            Entity hitbox = this->createHitboxFromNode(*hbNode, parent);
            frame.hitboxes.push_back(hitbox);
        }
        controller.frames.push_back(std::move(frame));
    }
    return controller;
}

Entity HitboxLoader::createHitboxFromNode(const DataNode& node, Entity parent) const
{
    auto posNode = node.has("position") ? node.getObject("position") : nullptr;
    Position offset = posNode ? DataUtils::parsePosition(*posNode) : Position{0.f, 0.f};

    int damage = node.getInt("damage", 0);
    DebugConfig debug = DataUtils::parseDebug(node, {false, Color{255, 0, 0, 255}});
    
    std::string shape = node.getString("shape", "rectangle");
    if (shape == "rectangle") return this->factory.createHitboxChild(EntityFactory::HitboxChildParams{
        parent, offset, damage, debug}, DataUtils::parseRect(node));
    if (shape == "circle") return this->factory.createHitboxChild(EntityFactory::HitboxChildParams{
        parent, offset, damage, debug}, DataUtils::parseCircle(node));
    throw std::runtime_error("Invalid hitbox shape: " + shape);
}
