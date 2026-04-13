#include "../include/AnimationLoader/AnimationLoader.h"

#include "../include/StateMapper/StateMapper.h"

#include <stdexcept>

std::unordered_map<StateId, Animation>
AnimationLoader::load(const std::string& path) const
{
    auto root = this->parser.parse(path);

    std::unordered_map<StateId, Animation> result;

    for (auto& node : root->getArray("animations"))
    {
        std::string stateStr = node->getString("state");
        StateId state = StateMapper::fromString(stateStr);

        if (state == StateId::Unknown)
        { throw std::runtime_error("Invalid state in animation: " + stateStr); }

        Animation anim;

        anim.frameDuration = node->getFloat("frameDuration");
        anim.loop = node->getBool("loop");

        for (auto& fnode : node->getArray("frames"))
        {
            AnimationFrame frame;
            frame.x = fnode->getInt("x");
            frame.y = fnode->getInt("y");
            frame.width = fnode->getInt("width");
            frame.height = fnode->getInt("height");

            anim.frames.push_back(frame);
        }

        result[state] = std::move(anim);
    }

    return result;
}