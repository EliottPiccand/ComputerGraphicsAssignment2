#pragma once

#include <memory>

#include "Components/Component.h"
#include "Components/Transform.h"
#include <Utils/GL.h>
#include "Utils/Time.h"

namespace component
{

// #define DISPLAY_AI_TARGETS_AIM_RAY

class AITurretControls : public Component
{
  private:
    std::shared_ptr<Transform> transform;
    glm::vec2 target;
    glm::vec2 targetNextPathPoint;
    Instant lastFireTry;

    void pickTargetNextPathPoint();

  public:
    AITurretControls();

    void initialize() override;
    void update(float deltaTime) override;

#if defined(_DEBUG) && defined(DISPLAY_AI_TARGETS_AIM_RAY)
    bool render() const override;
#endif
};

} // namespace component
