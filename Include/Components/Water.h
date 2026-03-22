#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <tuple>

#include "Components/Component.h"
#include "Utils/Constants.h"
#include "Utils/GL.h"

namespace component
{

class Water : public Component
{
  private:
    static constexpr const float WORLD_SUBDIVISION_SIZE = 8.0f;

    using WorldGrid = std::array<
        //                   height, vertical velocity
        std::array<std::tuple<float, float>, static_cast<size_t>(WORLD_WIDTH / WORLD_SUBDIVISION_SIZE)>,
        static_cast<size_t>(WORLD_HEIGHT / WORLD_SUBDIVISION_SIZE)>;

    std::unique_ptr<WorldGrid> water;

  public:
    Water();

    void displaceWaterVolume(const glm::vec2 &position, float radius);

    void update(float deltaTime) override;
    bool render() const override;
};

} // namespace component
