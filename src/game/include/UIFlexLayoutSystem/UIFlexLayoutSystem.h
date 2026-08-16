#ifndef ui_flex_layout_system_h
#define ui_flex_layout_system_h

#include "IFlexLayoutHandler/IFlexLayoutHandler.h"

#include "domain/include/Entity/Entity.h"

#include "engine/include/System/System.h"

#include <memory>
#include <vector>

class World;

class UIFlexLayoutSystem : public System
{
public:
    void update(UpdateContext& ctx) override;
    void addHandler(std::unique_ptr<IFlexLayoutHandler> handler);

private:
    std::vector<std::unique_ptr<IFlexLayoutHandler>> handlers;

    void layoutRecursive(World& world, Entity container);
    void layoutContainer(World& world, Entity container);
    std::vector<Entity> collectChildren(World& world, Entity parent) const;
    bool hasFlexParent(World& world, Entity entity) const;
};

#endif // ui_flex_layout_system_h
