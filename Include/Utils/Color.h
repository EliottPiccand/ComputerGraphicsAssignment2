#pragma once

#include <Utils/GL.h>

using Color = glm::vec4;

#define rgba(r, g, b, a)                                                                                               \
    Color                                                                                                              \
    {                                                                                                                  \
        r##.0f / 255.0f, g##.0f / 255.0f, b##.0f / 255.0f, a                                                           \
    }

#pragma region colors

constexpr const Color BACKGROUND_COLOR = rgba(189, 220, 230, 1);

constexpr const Color WATER_COLOR = rgba(84, 176, 204, 1);
constexpr const Color FOAM_COLOR = rgba(255, 255, 255, 1);

constexpr const Color RADAR_CONE_COLOR = rgba(255, 0, 0, 0.2);

constexpr const Color PLAYER_SHIP_FILL_COLOR = rgba(121, 211, 216, 1);
constexpr const Color PLAYER_SHIP_OUTLINE_COLOR = rgba(22, 96, 128, 1);

constexpr const Color ENEMY_SHIP_FILL_COLOR = rgba(123, 216, 121, 1);
constexpr const Color ENEMY_SHIP_OUTLINE_COLOR = rgba(24, 128, 22, 1);

constexpr const Color AIM_RAY_COLOR = rgba(220, 87, 87, 1);

constexpr const Color CANNONBALL_FILL_COLOR = rgba(237, 208, 81, 1);
constexpr const Color CANNONBALL_OUTLINE_COLOR = rgba(162, 138, 30, 1);

constexpr const Color EXPLOSION_FLASH_COLOR = rgba(242, 248, 100, 1);
constexpr const Color EXPLOSION_LAYER_0_COLOR = rgba(182, 45, 12, 1);
constexpr const Color EXPLOSION_LAYER_1_COLOR = rgba(208, 98, 34, 1);
constexpr const Color EXPLOSION_LAYER_2_COLOR = rgba(215, 164, 55, 1);

#pragma endregion colors
