#include "Components/Water.h"

#include <algorithm>
#include <ranges>

#include <GL/glew.h>

#include "Utils/Color.h"

constexpr float WATER_WAVES_SPEED = 100.0f; // m/s
constexpr float WATER_WAVES_DAMPING = 1.2f;

using namespace component;

Water::Water()
{
    water = std::make_unique<WorldGrid>();

    for (auto &row : *water)
    {
        row.fill(std::make_tuple(0.0f, 0.0f));
    }
}

void Water::displaceWaterVolume(const glm::vec2 &position, float radius)
{
    for (auto &&[y, row] : *water | std::views::enumerate)
    {
        for (auto &&[x, data] : row | std::views::enumerate)
        {
            auto &[height, velocity] = data;

            const glm::vec2 cellPosition{x * WORLD_SUBDIVISION_SIZE, y * WORLD_SUBDIVISION_SIZE};
            const float distance = glm::length(cellPosition - position);
            if (distance <= radius)
            {
                const float normalizedDistance = distance / radius;
                height = 0.5f * std::exp(1.0f - 1.0f / (1.0f - normalizedDistance * normalizedDistance));
                height *= radius;
                height = std::clamp(height, -WORLD_SUBDIVISION_SIZE * 10.0f, WORLD_SUBDIVISION_SIZE * 10.0f);
                velocity = height;
            }
        }
    }
}

void Water::update(float deltaTime)
{
    auto &water = *this->water;

    if (deltaTime > 1.0f / 30.0f)
    {
        // note: if the time step is too big, the CLF stability conditions are not met, causing the simulation to
        // diverge.
        return;
    }

    for (auto &&[y, row] : water | std::views::enumerate)
    {
        for (auto &&[x, data] : row | std::views::enumerate)
        {
            auto &[height, velocity] = data;

            const float heightLeft = 0 <= x - 1 ? std::get<0>(water[y][x - 1]) : 0.0f;
            const float heightRight = x + 1 < static_cast<long long>(row.size()) ? std::get<0>(water[y][x + 1]) : 0.0f;
            const float heightUp = 0 <= y - 1 ? std::get<0>(water[y - 1][x]) : 0.0f;
            const float heightDown = y + 1 < static_cast<long long>(water.size()) ? std::get<0>(water[y + 1][x]) : 0.0f;

            const float laplacian = (heightLeft + heightRight + heightUp + heightDown - 4.0f * height);
            velocity += laplacian * WATER_WAVES_SPEED * deltaTime;
            velocity -= velocity * WATER_WAVES_DAMPING * deltaTime;
        }
    }

    for (auto &&[y, row] : water | std::views::enumerate)
    {
        for (auto &&[x, data] : row | std::views::enumerate)
        {
            auto &[height, velocity] = data;

            height += velocity * deltaTime;
        }
    }
}

bool Water::render() const
{
    const auto &water = *this->water;

    for (auto &&[y, row] : water | std::views::enumerate)
    {
        for (auto &&[x, data] : row | std::views::enumerate)
        {
            auto &[height, _] = data;

            const auto color = glm::mix(WATER_COLOR, FOAM_COLOR, height / (WORLD_SUBDIVISION_SIZE * 3.0f));

            const auto xFloat = static_cast<float>(x);
            const auto yFloat = static_cast<float>(y);

            glColor3f(color.r, color.g, color.b);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glRectf(xFloat * WORLD_SUBDIVISION_SIZE, yFloat * WORLD_SUBDIVISION_SIZE,
                    (xFloat + 1.0f) * WORLD_SUBDIVISION_SIZE, (yFloat + 1.0f) * WORLD_SUBDIVISION_SIZE);
        }
    }

    return false;
}
