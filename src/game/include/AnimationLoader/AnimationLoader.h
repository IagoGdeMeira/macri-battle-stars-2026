#ifndef animation_loader_h
#define animation_loader_h

#include "../../domain/include/Animation/AnimationSet.h"
#include "../../domain/include/StateId/StateId.h"

#include "../../engine/include/DataParser/DataParser.h"

#include <memory>
#include <unordered_map>

class StateIdMapper;

class AnimationLoader
{
public:
    AnimationLoader(DataParser& parser) : parser(parser) {}

    AnimationSet load(const std::string& path) const;
    AnimationSet load(const std::string& path, const StateIdMapper& mapper) const;

private:
    DataParser& parser;
};

#endif // animation_loader_h
