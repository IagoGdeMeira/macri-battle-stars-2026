#ifndef ui_layer_builder_h
#define ui_layer_builder_h

#include "domain/include/Entity/Entity.h"
#include "domain/value_objects/Geometry/Geometry.h"

class DataNode;
class UIFactory;

class UILayerBuilder
{
public:
    explicit UILayerBuilder(UIFactory& factory) : factory(factory) {}
    void build(Entity container, const DataNode& groupNode, const Position& baseOffset, const Dimension2D& baseSize) const;

private:
    UIFactory& factory;

    Position resolveOffset(const DataNode& layer, const Position& base) const;
    Dimension2D resolveSize(const DataNode& layer, const Dimension2D& base) const;

    struct AddData { Entity container; const DataNode& layer; const Position& offset; };
    void addTexture(AddData data, const Dimension2D& size) const;
    void addRectangle(AddData data, const Dimension2D& size) const;
    void addCircle(AddData data, const Dimension2D& size) const;
    void addPolygon(AddData data) const;
};

#endif // ui_layer_builder_h
