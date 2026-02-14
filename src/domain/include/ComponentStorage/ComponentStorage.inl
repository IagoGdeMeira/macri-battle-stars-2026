#include <stdexcept>

template <typename ComponentType>
void ComponentStorage<ComponentType>::add(Entity e, const ComponentType &component)
{
    if (e.index()>= this->sparse.size()) this->sparse.resize(e.index() + 1);

    this->sparse[e.index()] = this->components.size();

    this->entities.push_back(e);
    this->components.push_back(component);
}

template <typename ComponentType>
void ComponentStorage<ComponentType>::remove(Entity e)
{
    if (!this->has(e))return;

    size_t index = this->sparse[e.index()];
    size_t last = this->components.size() - 1;

    if (index != last)
    {
        this->components[index] = std::move(this->components[last]);
        this->entities[index] = this->entities[last];

        this->sparse[this->entities[index].index()] = index;
    }

    this->components.pop_back();
    this->entities.pop_back();
}

template <typename ComponentType>
bool ComponentStorage<ComponentType>::has(Entity e) const
{
    return e.index() < this->sparse.size() &&
        this->sparse[e.index()] < this->entities.size() &&
        this->entities[this->sparse[e.index()]] == e;
}

template <typename ComponentType>
ComponentType &ComponentStorage<ComponentType>::get(Entity e)
{
    if (!this->has(e)) throw std::runtime_error("Component not found");
    return this->components[this->sparse[e.index()]];
}

template <typename ComponentType>
size_t ComponentStorage<ComponentType>::size() const
{ return this->components.size(); }

template <typename ComponentType>
std::vector<ComponentType> &ComponentStorage<ComponentType>::getComponents()
{ return this->components; }

template <typename ComponentType>
const std::vector<ComponentType> &ComponentStorage<ComponentType>::getComponents() const
{ return this->components; }

template <typename ComponentType>
const std::vector<Entity> &ComponentStorage<ComponentType>::getEntities() const
{ return this->entities; }
