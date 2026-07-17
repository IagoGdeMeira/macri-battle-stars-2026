#ifndef view_h
#define view_h

#include "ComponentManager/ComponentManager.h"
#include "ComponentStorage/ComponentStorage.h"

#include <tuple>
#include <utility>
#include <vector>

template <typename... Components>
class View
{
public:
    View(ComponentManager& componentManager);

    class Iterator
    {
    public:
        Iterator(
            ComponentManager& manager,
            const std::vector<Entity>& entities,
            size_t index,
            size_t baseIndex
        );

        Iterator& operator++();
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        auto operator*();

    private:
        ComponentManager& manager;
        const std::vector<Entity>& entities;
        size_t index;
        size_t baseIndex;

        void advance();

        bool matches(Entity e);

        template <size_t... I>
        bool matchesImpl(Entity e, std::index_sequence<I...>);
    };

    Iterator begin();
    Iterator end();

private:
    ComponentManager& manager;

    IComponentStorage* baseStorage;
    size_t baseIndex;
};

#include "View.inl"

#endif // view_h
