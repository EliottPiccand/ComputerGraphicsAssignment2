#include "GameObject.h"

#include <cassert>

#include <GL/glew.h>

GameObject::GameObject() : id(nextId)
{
    nextId += 1;
}

GameObjectId GameObject::getId() const
{
    return id;
}

std::shared_ptr<GameObject> GameObject::addChild()
{
    auto child = std::make_shared<GameObject>();
    child->parent = std::optional(shared_from_this());
    children.push_back(child);
    return child;
}

std::optional<std::shared_ptr<GameObject>> GameObject::getParent() const
{
    return parent;
}

std::optional<std::shared_ptr<GameObject>> GameObject::getGameObject(GameObjectId id)
{
    if (id == this->id)
    {
        return std::optional(shared_from_this());
    }

    for (auto &child : children)
    {
        auto obj = child->getGameObject(id);
        if (obj.has_value())
        {
            return obj;
        }
    }

    return std::nullopt;
}

std::shared_ptr<GameObject> GameObject::detach()
{
    while (!children.empty())
    {
        children[0]->detach();
    }

    if (parent.has_value())
    {
        auto &siblings = parent.value()->children;

        const auto it = std::find_if(siblings.begin(), siblings.end(), [&](auto o) { return o->id == id; });

        if (it != siblings.end())
        {
            siblings.erase(it);
        }

        parent = std::nullopt;
    }

    return shared_from_this();
}

void GameObject::initialize()
{
    if (initialized)
    {
        return;
    }

    for (auto &component : components)
    {
        component->initialize();
    }

    for (auto &child : children)
    {
        child->initialize();
    }

    initialized = true;
}

void GameObject::update(float deltaTime)
{
    if (!active)
    {
        return;
    }

    assert(initialized && "GameObject::update called while uninitialized");

    for (auto &child : children)
    {
        child->update(deltaTime);
    }

    for (auto &component : components)
    {
        component->update(deltaTime);
    }
}

void GameObject::render() const
{
    if (!visible)
    {
        return;
    }

    assert(initialized && "GameObject::render called while uninitialized");

    size_t matricesToPop = 0;

    for (const auto &component : components)
    {
        if (component->render())
        {
            matricesToPop += 1;
        }
    }

    for (const auto &child : children)
    {
        child->render();
    }

    while (matricesToPop > 0)
    {
        glPopMatrix();
        matricesToPop -= 1;
    }
}
