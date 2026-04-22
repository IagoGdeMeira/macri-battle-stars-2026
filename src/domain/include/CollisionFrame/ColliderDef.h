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
};

class RectangleDef : public ColliderDef
{
public:
    float width = 0.0f;
    float height = 0.0f;

    ColliderType getType() const override { return ColliderType::Rectangle; }
};

class CircleDef : public ColliderDef
{
public:
    float radius = 0.0f;

    ColliderType getType() const override { return ColliderType::Circle; }
};

#endif // collider_def_h
