#pragma once

#include <cstdint>

#include "Components/Component.h"
#include <Utils/GL.h>
#include "Utils/Time.h"

namespace component
{

class Camera : public Component
{
  private:
    float baseLeft;
    float baseRight;
    float baseBottom;
    float baseTop;

    glm::vec2 shakingOffset = {0.0f, 0.0f};
    Instant shakingLastUpdate;

    float screenWidth;
    float screenHeight;

  public:
    Camera(uint32_t screenWidth, uint32_t screenHeight);

    void onViewportResize(uint32_t width, uint32_t height);
    glm::vec2 toWorldPosition(const glm::vec2 &position) const;
    void shake();

    void update(float deltaTime) override;
    bool render() const override;
};

} // namespace component
