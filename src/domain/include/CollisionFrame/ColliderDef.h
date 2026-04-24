#ifndef collider_def_h
#define collider_def_h

class ColliderDef
{
public:
    enum class ColliderType { Rectangle, Circle };

    virtual ~ColliderDef() = default;

    float offsetX = 0.0f;
    float offsetY = 0.0f;

    virtual ColliderType getType() const = 0;

    virtual std::unique_ptr<ColliderDef> clone() const = 0;
};

class RectangleDef : public ColliderDef
{
public:
    float width = 0.0f;
    float height = 0.0f;

    ColliderType getType() const override { return ColliderType::Rectangle; }

    std::unique_ptr<ColliderDef> clone() const override
    {
        auto copy = std::make_unique<RectangleDef>();
        copy->offsetX = this->offsetX;
        copy->offsetY = this->offsetY;
        copy->width = this->width;
        copy->height = this->height;
        return copy;
    }
};

class CircleDef : public ColliderDef
{
public:
    float radius = 0.0f;

    ColliderType getType() const override { return ColliderType::Circle; }

    std::unique_ptr<ColliderDef> clone() const override
    {
        auto copy = std::make_unique<CircleDef>();
        copy->offsetX = this->offsetX;
        copy->offsetY = this->offsetY;
        copy->radius = this->radius;
        return copy;
    }
};

#endif // collider_def_h
