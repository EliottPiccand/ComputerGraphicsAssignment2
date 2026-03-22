#pragma once

#include <concepts>
#include <memory>
#include <optional>

class GameObject;

namespace component
{

class Component : public std::enable_shared_from_this<Component>
{

  protected:
    std::shared_ptr<GameObject> owner;

  public:
    virtual ~Component() = default;

    virtual void initialize();
    virtual void update(float deltaTime);
    virtual bool render() const; /// return true if a model-view matrix has been pushed onto OpenGL stack

    void setOwner(std::shared_ptr<GameObject> entity);
    std::shared_ptr<GameObject> getOwner() const;
};

} // namespace component
