#pragma once

#include <cstdint>

#include <glm/glm.hpp>

#include "Components/Component.h"
#include "Utils/Time.h"

namespace component
{

class Camera : public Component
{
  private:
    glm::vec2 shakingOffset = {0.0f, 0.0f};
    Instant shakingLastUpdate;

    float viewportWidth;
    float viewportHeight;

  public:
    glm::vec4 lrbt;

    Camera(const glm::vec4 &lrbt, uint32_t viewportWidth, uint32_t viewportHeight);

    void onViewportResize(uint32_t viewportWidth, uint32_t viewportHeight);
    static glm::vec4 getWorldLBRT(uint32_t viewportWidth, uint32_t viewportHeight);

    glm::vec2 toWorldPosition(const glm::vec2 &position) const;
    void shake();

    void update(float deltaTime) override;
    bool render() const override;
};

} // namespace component
