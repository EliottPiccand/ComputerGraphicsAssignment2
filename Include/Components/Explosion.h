#pragma once

#include <array>
#include <memory>

#include "Components/Component.h"
#include "Components/Transform.h"
#include "Utils/Color.h"

namespace component
{

class Explosion : public Component
{
  private:
    static inline constexpr const std::array COLORS = {
        EXPLOSION_LAYER_0_COLOR,
        EXPLOSION_LAYER_1_COLOR,
        EXPLOSION_LAYER_2_COLOR,
    };

    std::shared_ptr<Transform> transform;
    float radius;
    std::array<float, COLORS.size()> rotations;

  public:
    Explosion(float radius);

    void initialize() override;
    void update(float deltaTime) override;
    bool render() const override;
};

} // namespace component
