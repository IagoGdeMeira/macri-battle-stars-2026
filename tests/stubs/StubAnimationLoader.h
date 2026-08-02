#ifndef stub_animation_loader_h
#define stub_animation_loader_h

#include "StubDataParser.h"

#include "domain/value_objects/Animation/AnimationSet.h"

#include "game/include/AnimationLoader/AnimationLoader.h"

#include <string>

class StubAnimationLoader : public AnimationLoader
{
public:
    explicit StubAnimationLoader(DataParser& parser) : AnimationLoader(parser) {}

    StubAnimationLoader() : AnimationLoader(this->dummyParser) {}

    AnimationSet loadFromIndex(const std::string& indexPath, const StateIdMapper& mapper) const override
    { (void)indexPath; (void)mapper; return AnimationSet{}; }

private:
    StubDataParser dummyParser;
};

#endif // stub_animation_loader_h
