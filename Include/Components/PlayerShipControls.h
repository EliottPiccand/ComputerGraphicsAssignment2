#pragma once

#include <memory>

#include "Components/Component.h"
#include "Components/Transform.h"
#include "Utils/SpeedState.h"

namespace component
{

class PlayerShipControls : public Component
{
  private:
    std::weak_ptr<Transform> transform;
    SpeedState speedState = SpeedState::Stop;

  public:
    void initialize() override;
    void update(float deltaTime) override;
};

} // namespace component
