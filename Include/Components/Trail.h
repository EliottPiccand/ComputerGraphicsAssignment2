#pragma once

#include <cstddef>
#include <memory>
#include <optional>

#include <glm/glm.hpp>

#include "Components/Component.h"
#include "Components/Transform.h"
#include "Container/CyclicQueue.h"
#include "Utils/Color.h"

namespace component
{

class TrailRenderer;

class Trail : public Component
{
  private:
    static constexpr const size_t MAX_TRAIL_PARTICLES = 100;

    std::shared_ptr<TrailRenderer> renderer;

    Color color;
    std::optional<float> waterDisplacementRadius;
    std::shared_ptr<Transform> transform;

    struct TrailParticle
    {
        glm::vec2 position;
        float intensity;
    };

    friend TrailRenderer;
    CyclicQueue<TrailParticle, MAX_TRAIL_PARTICLES> particles;

  public:
    Trail(std::shared_ptr<TrailRenderer> renderer, const Color &color, std::optional<float> waterDisplacementRadius);

    void initialize() override;
    void update(float deltaTime) override;
};

} // namespace component
