#pragma once

#include <memory>
#include <optional>

#include <glm/glm.hpp>

#include "Components/Component.h"
#include "Components/Transform.h"

namespace component
{

class PlayerTurretControls : public Component
{
  private:
    std::weak_ptr<Transform> transform;
    std::optional<glm::vec2> target;
    bool aimingValidPosition;

  public:
    void initialize() override;
    void update(float deltaTime) override;
    bool render() const override;
};

} // namespace component
