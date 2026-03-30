#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "Components/Component.h"
#include "Components/Transform.h"
#include "Utils/SpeedState.h"
#include "Utils/Time.h"

namespace component
{

// #define DISPLAY_AI_MOVEMENT_TARGETS

class AIShipControls : public Component
{
  private:
    std::weak_ptr<Transform> transform;
    SpeedState speedState = SpeedState::Forward;

    Instant lastTurn;
    glm::vec2 target;

    void pickTarget();

  public:
    AIShipControls();

    void initialize() override;
    void update(float deltaTime) override;

#if defined(OE_DEBUG) && defined(DISPLAY_AI_MOVEMENT_TARGETS)
    bool render() const override;
#endif
};

} // namespace component
