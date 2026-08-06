#ifndef animation_loader_h
#define animation_loader_h

#include "domain/value_objects/Animation/AnimationSet.h"

#include "engine/include/DataParser/DataParser.h"

class StateIdMapper;

class AnimationLoader
{
public:
    AnimationLoader(DataParser& parser) : parser(parser) {}
    virtual AnimationSet loadFromIndex(const std::string& indexPath, const StateIdMapper& mapper) const;

private:
    DataParser& parser;
};

#endif // animation_loader_h
