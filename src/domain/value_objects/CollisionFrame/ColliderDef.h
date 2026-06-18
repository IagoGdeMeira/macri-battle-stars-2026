#ifndef collider_def_h
#define collider_def_h

#include "../Geometry/Geometry.h"

#include <memory>

class ColliderDef
{
public:
    enum class ColliderType { Rectangle, Circle };

    virtual ~ColliderDef() = default;

    Position offset { 0.f, 0.f };

    virtual ColliderType getType() const = 0;
    virtual std::unique_ptr<ColliderDef> clone() const = 0;
};

class RectangleDef : public ColliderDef
{
public:
    float width = 0.f;
    float height = 0.f;

    ColliderType getType() const override { return ColliderType::Rectangle; }

    std::unique_ptr<ColliderDef> clone() const override
    {
        auto copy = std::make_unique<RectangleDef>();
        copy->offset = this->offset;
        copy->width = this->width;
        copy->height = this->height;
        return copy;
    }
};

class CircleDef : public ColliderDef
{
public:
    float radius = 0.f;

    ColliderType getType() const override { return ColliderType::Circle; }

    std::unique_ptr<ColliderDef> clone() const override
    {
        auto copy = std::make_unique<CircleDef>();
        copy->offset = this->offset;
        copy->radius = this->radius;
        return copy;
    }
};

#endif // collider_def_h
