#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "Components/Component.h"
#include "Components/Transform.h"
#include "GameObject.h"

namespace component
{

class Cannonball : public Component
{
  private:
    std::weak_ptr<Transform> transform;
    glm::vec2 target;
    GameObjectId shooterId;

  public:
    Cannonball(const glm::vec2 &target, GameObjectId shooterId);

    [[nodiscard]] GameObjectId getShooterId() const;

    void initialize() override;
    void update(float deltaTime) override;
};

} // namespace component
