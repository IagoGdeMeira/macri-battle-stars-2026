#ifndef collision_clip_loader_h
#define collision_clip_loader_h

#include "../../../domain/include/CollisionFrame/CollisionFrame.h"
#include "../../../domain/include/Geometry/Geometry.h"
#include "../../../domain/include/StateId/StateId.h"

#include "../../../engine/include/DataParser/DataParser.h"

#include <memory>
#include <unordered_map>

class StateIdMapper;

class CollisionClipLoader
{
public:
    using ClipMap = std::unordered_map<StateId, CollisionClip, StateId::Hash>;

    explicit CollisionClipLoader(DataParser& parser) : parser(parser) {}
    
    ClipMap load(const std::string& path) const;
    ClipMap load(const std::string& path, const StateIdMapper& mapper) const;

private:
    DataParser& parser;

    std::unique_ptr<ColliderDef> parseCollider(const DataNode& node) const;
    HitboxDef parseHitbox(const DataNode& node) const;
    HurtboxDef parseHurtbox(const DataNode& node) const;
    PushboxDef parsePushbox(const DataNode& node) const;

    void parseOffset(Position& offset, const DataNode& node) const;
};

#endif // collision_clip_loader_h
