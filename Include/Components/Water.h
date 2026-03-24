#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <tuple>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Components/Component.h"
#include "Utils/Constants.h"

namespace component
{

class Water : public Component
{
  private:
    static constexpr const float WORLD_SUBDIVISION_SIZE = 8.0f;

    static constexpr const size_t GRID_WIDTH = static_cast<size_t>(WORLD_WIDTH / WORLD_SUBDIVISION_SIZE);
    static constexpr const size_t GRID_HEIGHT = static_cast<size_t>(WORLD_HEIGHT / WORLD_SUBDIVISION_SIZE);

    //                                                height, vertical velocity
    using WorldGrid = std::array<std::array<std::tuple<float, float>, GRID_WIDTH>, GRID_HEIGHT>;

    std::unique_ptr<WorldGrid> water;
    
    static constexpr const size_t CHANNELS = 4; // RGBA
    using Texture = std::array<uint8_t, GRID_WIDTH * GRID_HEIGHT * CHANNELS>;
    GLuint texture;
    std::unique_ptr<Texture> textureData;

  public:
    Water();

    void displaceWaterVolume(const glm::vec2 &position, float radius);

    void update(float deltaTime) override;
    bool render() const override;
};

} // namespace component
