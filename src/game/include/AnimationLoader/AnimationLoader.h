#ifndef animation_loader_h
#define animation_loader_h

#include "../../domain/include/Animation/Animation.h"
#include "../../domain/include/StateId/StateId.h"

#include "../../../engine/include/DataParser/DataParser.h"

#include <memory>

#include <unordered_map>

class StateIdMapper;

class AnimationLoader
{
public:
    AnimationLoader(DataParser& parser) : parser(parser) {}

    using AnimationMap = std::unordered_map<StateId, Animation, StateId::Hash>;

    AnimationMap load(const std::string& path) const;
    AnimationMap load(const std::string& path, const StateIdMapper& mapper) const;

private:
    DataParser& parser;
};

#endif // animation_loader_h
