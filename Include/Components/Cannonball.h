#pragma once

#include "Components/Component.h"
#include "Components/Transform.h"

namespace component
{

class Cannonball : public Component
{
  private:
    std::shared_ptr<Transform> transform;
    glm::vec2 target;

  public:
    Cannonball(const glm::vec2 &target);

    void initialize() override;
    void update(float deltaTime) override;
};

} // namespace component
