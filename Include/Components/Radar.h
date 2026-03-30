#pragma once

#include <memory>

#include "Components/Component.h"
#include "Components/Transform.h"

namespace component
{

class Radar : public Component
{
  private:
    std::weak_ptr<Transform> transform;

  public:
    void initialize() override;
    void update(float deltaTime) override;
};

} // namespace component
