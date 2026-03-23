#include "GameObject.h"

#include <cassert>

#include "Utils/GL.h"

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
    assert(initialized && "GameObject::render called while uninitialized");

    bool shouldPopMatrix = false;

    for (const auto &component : components)
    {
        if (component->render())
        {
            shouldPopMatrix = true;
        }
    }

    for (const auto &child : children)
    {
        child->render();
    }

    if (shouldPopMatrix)
    {
        glPopMatrix();
    }
}
