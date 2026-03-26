#include "Components/Camera.h"

#include <chrono>

#include <GL/glew.h>
#include <glm/gtx/rotate_vector.hpp>

#include "Utils/Constants.h"
#include "Utils/Profiling.h"
#include "Utils/Random.h"

constexpr const Duration SHAKING_UPDATE_INTERVAL = std::chrono::milliseconds(20);
constexpr const float SHAKING_INTENSITY = 10.0f; // m
constexpr const float SHAKING_DECAY_FACTOR = 0.9f;
constexpr const float SHAKING_SPREAD_ANGLE = 60.0f; // °

using namespace component;

#define LRBT lrbt.x + shakingOffset.x, lrbt.y + shakingOffset.x, lrbt.z + shakingOffset.y, lrbt.w + shakingOffset.y

Camera::Camera(const glm::vec4 &lrbt, uint32_t viewportWidth, uint32_t viewportHeight)
    : shakingLastUpdate(now() - SHAKING_UPDATE_INTERVAL), lrbt(lrbt)
{
    onViewportResize(viewportWidth, viewportHeight);
}

void Camera::onViewportResize(uint32_t viewportWidth, uint32_t viewportHeight)
{
    glViewport(0, 0, static_cast<GLsizei>(viewportWidth), static_cast<GLsizei>(viewportHeight));

    this->viewportWidth = static_cast<float>(viewportWidth);
    this->viewportHeight = static_cast<float>(viewportHeight);
}

glm::vec4 Camera::getWorldLBRT(uint32_t width, uint32_t height)
{
    constexpr const float WORLD_DISPLAY_MARGIN = 100.0f; // m

    constexpr float VIRTUAL_WORLD_WIDTH = WORLD_WIDTH + 2.0f * WORLD_DISPLAY_MARGIN;
    constexpr float VIRTUAL_WORLD_HEIGHT = WORLD_HEIGHT + 2.0f * WORLD_DISPLAY_MARGIN;

    constexpr float WORLD_ASPECT_RATIO = VIRTUAL_WORLD_WIDTH / VIRTUAL_WORLD_HEIGHT;

    float viewportWidth = static_cast<float>(width);
    float viewportHeight = static_cast<float>(height);
    const float aspectRatio = viewportWidth / viewportHeight;

    glm::vec4 lrbt;

    if (aspectRatio > WORLD_ASPECT_RATIO)
    {
        lrbt.w = -WORLD_DISPLAY_MARGIN;
        lrbt.z = VIRTUAL_WORLD_HEIGHT - WORLD_DISPLAY_MARGIN;

        const float world_display_width = VIRTUAL_WORLD_WIDTH * viewportHeight / VIRTUAL_WORLD_HEIGHT;
        const float gap_display_width = (viewportWidth - world_display_width) / 2.0f;
        const float gap_world_width = gap_display_width * VIRTUAL_WORLD_HEIGHT / viewportHeight;

        lrbt.x = -gap_world_width - WORLD_DISPLAY_MARGIN;
        lrbt.y = VIRTUAL_WORLD_WIDTH + gap_world_width - WORLD_DISPLAY_MARGIN;
    }
    else
    {
        lrbt.x = -WORLD_DISPLAY_MARGIN;
        lrbt.y = VIRTUAL_WORLD_WIDTH - WORLD_DISPLAY_MARGIN;

        const float world_display_height = VIRTUAL_WORLD_HEIGHT * viewportWidth / VIRTUAL_WORLD_WIDTH;
        const float gap_display_height = (viewportHeight - world_display_height) / 2.0f;
        const float gap_world_height = gap_display_height * VIRTUAL_WORLD_WIDTH / viewportWidth;

        lrbt.w = -gap_world_height - WORLD_DISPLAY_MARGIN;
        lrbt.z = VIRTUAL_WORLD_HEIGHT + gap_world_height - WORLD_DISPLAY_MARGIN;
    }

    return lrbt;
}

void Camera::shake()
{
    shakingOffset = glm::rotate(glm::vec2(SHAKING_INTENSITY, 0.0f), glm::radians(Random::random(-180.0f, 180.0f)));
    shakingLastUpdate = now();
}

glm::vec2 Camera::toWorldPosition(const glm::vec2 &position) const
{
    const glm::vec2 normalizedPosition =
        glm::vec2(position.x, viewportHeight - position.y) / glm::vec2(viewportWidth, viewportHeight) * 2.0f - 1.0f;
    const glm::vec4 worldPosition =
        glm::inverse(glm::ortho(LRBT)) * glm::vec4(normalizedPosition.x, normalizedPosition.y, 0.0, 1.0);
    return {worldPosition.x, worldPosition.y};
}

void Camera::update(float deltaTime)
{
    ProfileScope;

    if (now() - shakingLastUpdate > SHAKING_UPDATE_INTERVAL)
    {
        shakingOffset =
            glm::rotate(shakingOffset,
                        glm::radians(180.0f + Random::random(-SHAKING_SPREAD_ANGLE, SHAKING_SPREAD_ANGLE))) *
            SHAKING_DECAY_FACTOR;
        shakingLastUpdate = now();
    }
}

bool Camera::render() const
{
    ProfileScope;
    ProfileScopeGPU("Camera::render");

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    glOrtho(LRBT, -1.0, 1.0);

    return true;
}
