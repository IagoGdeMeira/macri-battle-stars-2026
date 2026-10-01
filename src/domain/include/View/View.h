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
        struct Config { ComponentManager& manager; const std::vector<Entity>& entities; size_t index, baseIndex; };

        Iterator(Config cfg) : cfg(cfg) { this->advance(); }

        Iterator& operator++();
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        auto operator*();

    private:
        Config cfg;

        void advance();
        bool matches(Entity e);
        template <size_t... I>
        bool matchesImpl(Entity e, std::index_sequence<I...>);
    };

    size_t size() const;

    Iterator begin() { return Iterator({ this->manager, this->baseStorage->entities(), 0, this->baseIndex }); }
    Iterator end() { return Iterator({ this->manager, this->baseStorage->entities(), this->baseStorage->entities().size(), this->baseIndex }); }

private:
    ComponentManager& manager;
    IComponentStorage* baseStorage;
    size_t baseIndex;

    bool matchesEntity(Entity e) const;
};

#include "View.inl"

#endif // view_h
