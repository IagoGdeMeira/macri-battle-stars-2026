#include "HitboxLoader/HitboxLoader.h"

#include "EntityFactory/EntityFactory.h"

#include "domain/include/Entity/Entity.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include "engine/include/DataUtils/DataUtils.h"

#include <stdexcept>

HitboxControllerComponent HitboxLoader::load(const std::string& path, Entity parent, bool facingLeft)
{
    auto root = this->parser.parse(path);
    HitboxControllerComponent controller;
    controller.loop = root->getBool("loop", false);

    for (auto& frameNode : root->getArray("frames"))
    {
        HitboxControllerComponent::Frame frame;
        frame.duration = frameNode->getFloat("duration", 0.f);

        for (auto& hbNode : frameNode->getArray("hitboxes"))
        {
            std::string type = hbNode->getString("type", "rectangle");
            Position offset = DataUtils::parsePosition(*hbNode, Position{0.f, 0.f});
            int damage = hbNode->getInt("damage", 0);

            if (type == "rectangle")
            {
                Rectangle rect = DataUtils::parseRect(*hbNode);
                Entity hitbox = this->factory.createHitboxChild(EntityFactory::HitboxChildParams{
                    parent, offset, damage, facingLeft}, rect);
                frame.hitboxes.push_back(hitbox);
            }
            else if (type == "circle")
            {
                Circle circle = DataUtils::parseCircle(*hbNode);
                Entity hitbox = this->factory.createHitboxChild(EntityFactory::HitboxChildParams{
                    parent, offset, damage, facingLeft}, circle);
                frame.hitboxes.push_back(hitbox);
            }
            else throw std::runtime_error("Invalid hitbox type: " + type);
        }
        controller.frames.push_back(std::move(frame));
    }

    return controller;
}
