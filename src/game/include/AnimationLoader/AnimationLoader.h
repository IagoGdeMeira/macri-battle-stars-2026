#ifndef animation_loader_h
#define animation_loader_h

#include "../../domain/include/Animation/Animation.h"
#include "../../domain/include/StateId/StateId.h"

#include "../../../engine/include/DataParser/DataParser.h"

#include <unordered_map>

class AnimationLoader
{
public:
    AnimationLoader(DataParser& parser) : parser(parser) {}

    std::unordered_map<StateId, Animation> load(const std::string& path) const;

private:
    DataParser& parser;
};

#endif // animation_loader_h
