#pragma once

#include <memory>
#include <optional>

#include "Components/Component.h"
#include "Components/Transform.h"
#include "Utils/GL.h"

namespace component
{

class PlayerTurretControls : public Component
{
  private:
    std::shared_ptr<Transform> transform;
    std::optional<glm::vec2> target;
    bool aimingValidPosition;

  public:
    void initialize() override;
    void update(float deltaTime) override;
    bool render() const override;
};

} // namespace component