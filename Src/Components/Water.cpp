#include "Components/Water.h"

#include <algorithm>
#include <memory>
#include <ranges>

#include "Utils/Color.h"
#include "Utils/Constants.h"
#include "Utils/Profiling.h"

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

    textureData = std::make_unique<Texture>();
    for (auto &c : *textureData)
    {
        c = 255;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GRID_WIDTH, GRID_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData->data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    ProfileScope;

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

            const glm::vec3 color = glm::mix(WATER_COLOR, FOAM_COLOR, height / (WORLD_SUBDIVISION_SIZE * 3.0f));
            const size_t pixelIndex = (y * GRID_WIDTH + x) * CHANNELS;
            (*textureData)[pixelIndex + 0] = static_cast<uint8_t>(color.r * 255.0f);
            (*textureData)[pixelIndex + 1] = static_cast<uint8_t>(color.g * 255.0f);
            (*textureData)[pixelIndex + 2] = static_cast<uint8_t>(color.b * 255.0f);
        }
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GRID_WIDTH, GRID_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, textureData->data());
}

bool Water::render() const
{
    ProfileScope;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(WORLD_WIDTH, 0.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(WORLD_WIDTH, WORLD_HEIGHT);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, WORLD_HEIGHT);
        glEnd();
    glDisable(GL_TEXTURE_2D);

    return false;
}
