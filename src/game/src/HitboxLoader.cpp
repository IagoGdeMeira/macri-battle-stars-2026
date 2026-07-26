#include "HitboxLoader/HitboxLoader.h"

#include "EntityFactory/EntityFactory.h"

#include "domain/value_objects/StateId/StateId.h"

#include "engine/utils/DataUtils/DataUtils.h"
#include "engine/value_objects/DebugConfig/DebugConfig.h"

#include <stdexcept>

HitboxLoader::ControllerMap HitboxLoader::load(const DataNode& root, Entity parent, bool facingLeft) const
{
    ControllerMap result;

    if (!root.has("states")) return result;
    for (auto& stateNode : root.getArray("states"))
    {
        std::string stateName = stateNode->getString("name");
        StateId state = StateId::fromBaseName(stateName);

        HitboxControllerComponent controller;
        controller.loop = stateNode->getBool("loop", false);

        for (auto& frameNode : stateNode->getArray("frames"))
        {
            HitboxControllerComponent::Frame frame;
            frame.duration = frameNode->getFloat("duration", 0.f);

            for (auto& hbNode : frameNode->getArray("hitboxes"))
            {
                Entity hitbox = this->createHitboxFromNode(*hbNode, parent, facingLeft);
                frame.hitboxes.push_back(hitbox);
            }
            controller.frames.push_back(std::move(frame));
        }
        result[state] = std::move(controller);
    }
    return result;
}

Entity HitboxLoader::createHitboxFromNode(const DataNode& node, Entity parent, bool facingLeft) const
{
    Position offset     = DataUtils::parsePosition(node, Position{0.f, 0.f});
    int damage          = node.getInt("damage", 0);
    DebugConfig debug   = DataUtils::parseDebug(node, {false, Color{255, 0, 0, 128}});
    
    std::string type = node.getString("type", "rectangle");
    if (type == "rectangle") return this->factory.createHitboxChild(EntityFactory::HitboxChildParams{
        parent, offset, damage, facingLeft, debug}, DataUtils::parseRect(node));

    if (type == "circle") return this->factory.createHitboxChild(EntityFactory::HitboxChildParams{
        parent, offset, damage, facingLeft, debug}, DataUtils::parseCircle(node));

    throw std::runtime_error("Invalid hitbox type: " + type);
}
