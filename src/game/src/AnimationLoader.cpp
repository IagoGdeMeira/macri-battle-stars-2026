#include "../include/AnimationLoader/AnimationLoader.h"

#include "../include/StateIdMapper/StateIdMapper.h"
#include "../include/StateMapper/StateMapper.h"

#include <stdexcept>

AnimationSet AnimationLoader::load(const std::string& path) const
{
    StateIdMapper mapper;
    return this->load(path, mapper);
}

AnimationSet AnimationLoader::load(const std::string& path, const StateIdMapper& mapper) const
{
    auto root = parser.parse(path);
    AnimationSet set;

    for (auto& node : root->getArray("animations"))
    {
        std::string stateStr = node->getString("state");
        StateId state = mapper.fromString(stateStr);
        if (state == StateId::Unknown) throw std::runtime_error("Invalid state in animation: " + stateStr);

        Animation anim;
        anim.frameDuration = node->getFloat("frameDuration");
        anim.loop = node->getBool("loop");
        for (auto& fnode : node->getArray("frames"))
        {
            Animation::Frame frame;
            frame.x = fnode->getInt("x");
            frame.y = fnode->getInt("y");
            frame.width = fnode->getInt("width");
            frame.height = fnode->getInt("height");
            anim.frames.push_back(frame);
        }

        std::string direction = node->getString("direction", "Right");
        if (direction == "Left")
        {
            set.left[state] = std::move(anim);
            set.symmetric = false;
        }
        else set.right[state] = std::move(anim);    
    }

    return set;
}