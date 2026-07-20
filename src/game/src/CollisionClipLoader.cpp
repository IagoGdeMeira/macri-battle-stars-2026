#include "CollisionClipLoader/CollisionClipLoader.h"

#include "StateIdMapper/StateIdMapper.h"

#include <stdexcept>

CollisionClipLoader::ClipMap CollisionClipLoader::load(const std::string& path) const
{
    StateIdMapper defaultMapper;
    return this->load(path, defaultMapper);
}

CollisionClipLoader::ClipMap CollisionClipLoader::load(
    const std::string& path, const StateIdMapper& mapper
) const {
    auto root = parser.parse(path);
    ClipMap result;

    const std::string clipsKey = root->has("collisionClips") ? "collisionClips" : "clips";
    for (auto& clipNode : root->getArray(clipsKey))
    {
        std::string stateStr = clipNode->getString("state");
        StateId state = mapper.fromString(stateStr);

        if (state == StateId::Unknown)
        { throw std::runtime_error("Unknown state in collision clip: " + stateStr); }

        CollisionClip clip;
        clip.loop = clipNode->getBool("loop", false);

        for (auto& frameNode : clipNode->getArray("frames"))
        {
            CollisionFrame frame;
            frame.duration = frameNode->getFloat("duration");

            if (frameNode->has("hitboxes")) for (auto& node : frameNode->getArray("hitboxes"))
            { frame.hitboxes.push_back(this->parseHitbox(*node)); }

            if (frameNode->has("hurtboxes")) for (auto& node : frameNode->getArray("hurtboxes"))
            { frame.hurtboxes.push_back(this->parseHurtbox(*node)); }

            if (frameNode->has("pushboxes")) for (auto& node : frameNode->getArray("pushboxes"))
            { frame.pushboxes.push_back(this->parsePushbox(*node)); }

            clip.frames.push_back(std::move(frame));
        }

        result[state] = std::move(clip);
    }

    return result;
}

std::unique_ptr<ColliderDef> CollisionClipLoader::parseCollider(const DataNode& node) const
{
    if (node.has("radius"))
    {
        auto circle = std::make_unique<CircleDef>();
        circle->radius = node.getFloat("radius");
        this->parseOffset(circle->offset, node);
        return circle;
    } 
    else if (node.has("width") && node.has("height"))
    {
        auto rect = std::make_unique<RectangleDef>();
        rect->width = node.getFloat("width");
        rect->height = node.getFloat("height");
        this->parseOffset(rect->offset, node);
        return rect;
    } 
    else throw std::runtime_error("Collider must have either 'radius' or 'width'/'height'");
}

HitboxDef CollisionClipLoader::parseHitbox(const DataNode& node) const
{
    HitboxDef def;
    def.collider = this->parseCollider(node);
    def.damage = node.getInt("damage", 0);
    return def;
}

HurtboxDef CollisionClipLoader::parseHurtbox(const DataNode& node) const
{
    HurtboxDef def;
    def.collider = this->parseCollider(node);
    def.damageMultiplier = node.getFloat("damageMultiplier", 1.f);
    return def;
}

PushboxDef CollisionClipLoader::parsePushbox(const DataNode& node) const
{
    PushboxDef def;
    def.collider = this->parseCollider(node);
    def.mass = node.getFloat("mass", 1.f);
    def.pushResistance = node.getFloat("pushResistance", 1.f);
    def.type = node.getString("type", "Dynamic") == "Dynamic"
        ? PushboxDef::Type::Dynamic
        : PushboxDef::Type::Static;
    return def;
}

void CollisionClipLoader::parseOffset(Position& offset, const DataNode& node) const
{
    if (!node.has("offset")) return;

    auto offsetNode = node.getObject("offset");
    if (!offsetNode) throw std::runtime_error("'offset' must be an object with keys 'x' and 'y'");

    offset.x = offsetNode->getFloat("x", 0.f);
    offset.y = offsetNode->getFloat("y", 0.f);
}
