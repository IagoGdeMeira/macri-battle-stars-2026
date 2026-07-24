#include "HurtboxLoader/HurtboxLoader.h"

#include "EntityFactory/EntityFactory.h"

#include "domain/value_objects/Geometry/Geometry.h"

#include "engine/include/DataUtils/DataUtils.h"

#include <stdexcept>

HurtboxControllerComponent HurtboxLoader::load(const std::string& path, Entity parent, bool facingLeft)
{
    auto root = this->parser.parse(path);
    HurtboxControllerComponent controller;
    controller.loop = root->getBool("loop", false);

    for (auto& frameNode : root->getArray("frames"))
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

    return controller;
}

Entity HurtboxLoader::createHurtboxFromNode(const DataNode& node, Entity parent, bool facingLeft)
{
    std::string type = node.getString("type", "rectangle");
    Position offset = DataUtils::parsePosition(node, Position{0.f, 0.f});
    float damageMultiplier = node.getFloat("damageMultiplier", 1.f);

    if (type == "rectangle")
    {
        Rectangle rect = DataUtils::parseRect(node);
        return this->factory.createHurtboxChild(EntityFactory::HurtboxChildParams{
            parent, offset, damageMultiplier, facingLeft}, rect);
    }
    if (type == "circle")
    {
        Circle circle = DataUtils::parseCircle(node);
        return this->factory.createHurtboxChild(EntityFactory::HurtboxChildParams{
            parent, offset, damageMultiplier, facingLeft}, circle);
    }
    throw std::runtime_error("Invalid hurtbox type: " + type);
}
