#include "HitboxLoader/HitboxLoader.h"

#include "EntityFactory/EntityFactory.h"

#include "domain/utils/Logger/Logger.h"
#include "domain/value_objects/StateId/StateId.h"

#include "engine/utils/DataUtils/DataUtils.h"
#include "engine/value_objects/DebugConfig/DebugConfig.h"

#include <stdexcept>

HitboxLoader::ControllerMap HitboxLoader::load(const DataNode& root, Entity parent) const
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
                Entity hitbox = this->createHitboxFromNode(*hbNode, parent);
                frame.hitboxes.push_back(hitbox);
            }
            controller.frames.push_back(std::move(frame));
        }
        result[state] = std::move(controller);
    }
    return result;
}

Entity HitboxLoader::createHitboxFromNode(const DataNode& node, Entity parent) const
{
    LOG_DEBUG("HitboxLoader: node has 'position'={}, has 'size'={}, has 'debug'={}",
        node.has("position"), node.has("size"), node.has("debug"));
    LOG_DEBUG("HitboxLoader: node has 'x'={}, has 'y'={}, has 'width'={}, has 'height'={}",
        node.has("x"), node.has("y"), node.has("width"), node.has("height"));

    auto posNode = node.has("position") ? node.getObject("position") : nullptr;
    Position offset = posNode ? DataUtils::parsePosition(*posNode) : Position{0.f, 0.f};

    int damage = node.getInt("damage", 0);
    DebugConfig debug = DataUtils::parseDebug(node, {false, Color{255, 0, 0, 255}});
    
    std::string type = node.getString("type", "rectangle");
    if (type == "rectangle")
    {
        Rectangle rect = DataUtils::parseRect(node);
        LOG_DEBUG("HitboxLoader: parsed rect size=({},{}) offset=({},{}) debug.color=({},{},{},{})",
            rect.size.width, rect.size.height, offset.x, offset.y,
            static_cast<int>(debug.color.r), static_cast<int>(debug.color.g),
            static_cast<int>(debug.color.b), static_cast<int>(debug.color.a));
        return this->factory.createHitboxChild(EntityFactory::HitboxChildParams{
            parent, offset, damage, debug}, rect);
    }
    if (type == "circle")
    {
        Circle circle = DataUtils::parseCircle(node);
        LOG_DEBUG("HitboxLoader: parsed circle radius={} offset=({},{})", circle.radius, offset.x, offset.y);
        return this->factory.createHitboxChild(EntityFactory::HitboxChildParams{
            parent, offset, damage, debug}, circle);
    }

    throw std::runtime_error("Invalid hitbox type: " + type);
}
