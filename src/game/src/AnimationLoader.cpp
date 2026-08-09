#include "AnimationLoader/AnimationLoader.h"
#include "StateIdMapper/StateIdMapper.h"
#include <stdexcept>

AnimationSet AnimationLoader::loadFromIndex(const std::string& indexPath, const StateIdMapper& mapper) const
{
    auto indexRoot = this->parser.parse(indexPath);
    AnimationSet set;

    for (auto& entry : indexRoot->getArray("states"))
    {
        std::string stateName = entry->getString("name");
        std::string filePath = entry->getString("path");
        StateId stateId = mapper.fromString(stateName);
        if (stateId == StateId::Unknown) throw std::runtime_error("Invalid state in animation index: " + stateName);

        auto animRoot = this->parser.parse(filePath);

        Animation anim;
        anim.frameDuration = animRoot->getFloat("frameDuration", 0.1f);
        anim.loop = animRoot->getBool("loop", true);

        for (auto& fnode : animRoot->getArray("frames"))
        {
            Animation::Frame frame;
            frame.x = fnode->getInt("x");
            frame.y = fnode->getInt("y");
            frame.width = fnode->getInt("width");
            frame.height = fnode->getInt("height");
            anim.frames.push_back(frame);
        }

        std::string direction = entry->getString("direction", "Right");
        bool asymmetric = entry->getBool("asymmetric", false);

        if (asymmetric)
        {
            set.symmetric = false;
            if (direction == "Left") set.left[stateId] = std::move(anim);
            else set.right[stateId] = std::move(anim);
        }
        else set.right[stateId] = std::move(anim);
    }
    return set;
}
