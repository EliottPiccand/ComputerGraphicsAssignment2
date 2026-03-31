#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

// *************
// *   World   *
// *************

constexpr const float WORLD_WIDTH = 1000.0f;  // m
constexpr const float WORLD_HEIGHT = 1000.0f; // m

// *************
// *   Ships   *
// *************

constexpr const float SHIP_SPEED = 100.0f; // m/s
constexpr const float SHIP_ROTATION_ANGLE = glm::radians(15.0f); // rad

constexpr const GLfloat SHIP_OUTLINE_WIDTH = 5.0f;
constexpr const float RECOIL_AMPLITUDE = 0.2f; // m
constexpr const float RECOIL_DECAY_INTENSITY = 0.9f;
