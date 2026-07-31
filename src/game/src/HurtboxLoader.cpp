#include "HurtboxLoader/HurtboxLoader.h"

#include "EntityFactory/EntityFactory.h"

#include "domain/utils/Logger/Logger.h"
#include "domain/value_objects/StateId/StateId.h"

#include "engine/utils/DataUtils/DataUtils.h"
#include "engine/value_objects/DebugConfig/DebugConfig.h"

#include <stdexcept>

HurtboxLoader::ControllerMap HurtboxLoader::load(const DataNode& root, Entity parent) const
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
                Entity hurtbox = this->createHurtboxFromNode(*hbNode, parent);
                frame.hurtboxes.push_back(hurtbox);
            }
            controller.frames.push_back(std::move(frame));
        }
        result[state] = std::move(controller);
    }
    return result;
}

Entity HurtboxLoader::createHurtboxFromNode(const DataNode& node, Entity parent) const
{
    LOG_DEBUG("HurtboxLoader: node has 'position'={}, has 'size'={}, has 'debug'={}",
        node.has("position"), node.has("size"), node.has("debug"));
    LOG_DEBUG("HurtboxLoader: node has 'x'={}, has 'y'={}, has 'width'={}, has 'height'={}",
        node.has("x"), node.has("y"), node.has("width"), node.has("height"));

    auto posNode = node.has("position") ? node.getObject("position") : nullptr;
    Position offset = posNode ? DataUtils::parsePosition(*posNode) : Position{0.f, 0.f};

    float damageMultiplier = node.getFloat("damageMultiplier", 0.f);
    DebugConfig debug = DataUtils::parseDebug(node, {false, Color{0, 255, 0, 255}});

    std::string type = node.getString("type", "rectangle");
    if (type == "rectangle")
    {
        Rectangle rect = DataUtils::parseRect(node);
        LOG_DEBUG("HurtboxLoader: parsed rect size=({},{}) offset=({},{}) debug.color=({},{},{},{})",
            rect.size.width, rect.size.height, offset.x, offset.y,
            static_cast<int>(debug.color.r), static_cast<int>(debug.color.g),
            static_cast<int>(debug.color.b), static_cast<int>(debug.color.a));
        return this->factory.createHurtboxChild(EntityFactory::HurtboxChildParams{
            parent, offset, damageMultiplier, debug}, rect);
    }
    if (type == "circle")
    {
        Circle circle = DataUtils::parseCircle(node);
        LOG_DEBUG("HurtboxLoader: parsed circle radius={} offset=({},{})", circle.radius, offset.x, offset.y);
        return this->factory.createHurtboxChild(EntityFactory::HurtboxChildParams{
            parent, offset, damageMultiplier, debug}, circle);
    }

    throw std::runtime_error("Invalid hurtbox type: " + type);
}
