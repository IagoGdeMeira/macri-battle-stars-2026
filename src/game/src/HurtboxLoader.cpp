#include "HurtboxLoader/HurtboxLoader.h"

#include "EntityFactory/EntityFactory.h"

#include "domain/value_objects/StateId/StateId.h"

#include "engine/utils/DataUtils/DataUtils.h"

#include <stdexcept>

HurtboxLoader::ControllerMap HurtboxLoader::load(const DataNode& root, Entity parent, bool facingLeft) const
{
    ControllerMap result;

    if (!root.has("states")) return result;
    for (auto& stateNode : root.getArray("states"))
    {
        std::string stateName = stateNode->getString("name");
        StateId state = StateId::fromBaseName(stateName);

        HurtboxControllerComponent controller;
        controller.loop = stateNode->getBool("loop", false);

        for (auto& frameNode : stateNode->getArray("frames"))
        {
            HurtboxControllerComponent::Frame frame;
            frame.duration = frameNode->getFloat("duration", 0.f);

            for (auto& hbNode : frameNode->getArray("hurtboxes"))
            {
                Entity hurtbox = this->createHurtboxFromNode(*hbNode, parent, facingLeft);
                frame.hurtboxes.push_back(hurtbox);
            }
            controller.frames.push_back(std::move(frame));
        }
        result[state] = std::move(controller);
    }
    return result;
}

Entity HurtboxLoader::createHurtboxFromNode(const DataNode& node, Entity parent, bool facingLeft) const
{
    Position offset = DataUtils::parsePosition(node, Position{0.f, 0.f});
    float damageMultiplier = node.getFloat("damageMultiplier", 0.f);

    std::string type = node.getString("type", "rectangle");
    if (type == "rectangle") return this->factory.createHurtboxChild(EntityFactory::HurtboxChildParams{
        parent, offset, damageMultiplier, facingLeft}, DataUtils::parseRect(node));
    
    if (type == "circle") return this->factory.createHurtboxChild(EntityFactory::HurtboxChildParams{
        parent, offset, damageMultiplier, facingLeft}, DataUtils::parseCircle(node));
        
    throw std::runtime_error("Invalid hurtbox type: " + type);
}
