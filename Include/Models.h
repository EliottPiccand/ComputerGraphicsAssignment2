#pragma once

#include <array>

#include <GL/glew.h>

#include "Components/Mesh.h"
#include "Utils/Color.h"
#include "Utils/Concepts.h"

namespace draw
{

namespace
{

}

template <RangeOf<glm::vec2> T> constexpr component::Mesh::RenderCallback polygon(const T &vertices)
{
    return [&](const std::shared_ptr<component::Theme> theme) {
        const auto &color = theme->getFillColor();

        glColor3f(color.r, color.g, color.b);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_POLYGON);
        for (const auto &vertex : vertices)
        {
            glVertex2f(vertex.x, vertex.y);
        }
        glEnd();

        const auto &outline = theme->getOutline();
        if (outline.has_value()) {
            glColor3f(outline->color.r, outline->color.g, outline->color.b);
            glLineWidth(outline->width);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBegin(GL_POLYGON);
            for (const auto &vertex : vertices)
            {
                glVertex2f(vertex.x, vertex.y);
            }
            glEnd();
        }
    };
}

void dashedArrow(const glm::vec2 &start, const glm::vec2 &end, const Color &color, float width, float tipSize);

} // namespace draw

#pragma region vertices

constexpr const std::array SHIP_RADAR_CENTER_VERTICES = {
    glm::vec2{0.1f, 0.0f},
    glm::vec2{0.092388f, 0.0382683f},
    glm::vec2{0.0707107f, 0.0707107f},
    glm::vec2{0.0382683f, 0.092388f},
    glm::vec2{0.0f, 0.1f},
    glm::vec2{-0.0382684f, 0.092388f},
    glm::vec2{-0.0707107f, 0.0707107f},
    glm::vec2{-0.092388f, 0.0382683f},
    glm::vec2{-0.1f, -0.0f},
    glm::vec2{-0.092388f, -0.0382683f},
    glm::vec2{-0.0707107f, -0.0707107f},
    glm::vec2{-0.0382683f, -0.092388f},
    glm::vec2{0.0f, -0.1f},
    glm::vec2{0.0382684f, -0.0923879f},
    glm::vec2{0.0707107f, -0.0707107f},
    glm::vec2{0.092388f, -0.0382683f},
};

constexpr const std::array SHIP_RADAR_CONE_VERTICES = {
    glm::vec2{0.7f, 0.0f},           glm::vec2{0.698501f, 0.0457822f}, glm::vec2{0.694011f, 0.0913683f},
    glm::vec2{0.68655f, 0.136563f},  glm::vec2{0.676148f, 0.181173f},  glm::vec2{0.662851f, 0.225008f},
    glm::vec2{0.646716f, 0.267878f}, glm::vec2{0.627811f, 0.309602f},  glm::vec2{0.606218f, 0.35f},
    glm::vec2{0.582029f, 0.388899f}, glm::vec2{0.555347f, 0.426133f},  glm::vec2{0.526288f, 0.461542f},
    glm::vec2{0.494975f, 0.494975f}, glm::vec2{0.461542f, 0.526288f},  glm::vec2{0.426133f, 0.555347f},
    glm::vec2{0.388899f, 0.582029f}, glm::vec2{0.35f, 0.606218f},      glm::vec2{0.0f, 0.0f},
};

constexpr const std::array SHIP_VERTICES = std::array{
    glm::vec2{-0.45f, 1.0f}, // Bottom-left
    glm::vec2{0.45f, 1.0f},  // Bottom-right
    glm::vec2{0.5f, -0.4f},  // Top-right
    glm::vec2{0.0f, -1.2f},  // Top
    glm::vec2{-0.5f, -0.4f}, // Top-left
};

constexpr const std::array SHIP_TURRET_VERTICES = std::array{
    glm::vec2(-0.25f, 0.2f), // Bottom-left
    glm::vec2(0.25f, 0.2f),  // Bottom-right
    glm::vec2(0.0f, -0.4f),  // Top
};

constexpr const std::array CANNONBALL_VERTICES = std::array{
    glm::vec2{-0.5f, 0.6f},  // Bottom-left
    glm::vec2{0.5f, 0.6f},   // Bottom-right
    glm::vec2{0.5f, -0.4f},  // Top-right
    glm::vec2{0.0f, -1.0f},  // Top
    glm::vec2{-0.5f, -0.4f}, // Top-left
};

constexpr const std::array EXPLOSION_VERTICES = [] {
    constexpr const glm::vec2 CANVAS_SIZE{100.0f, 100.0f};

    constexpr const glm::vec2 V1{41.0f, 2.0f};
    constexpr const glm::vec2 V2{37.0f, 23.0f};
    constexpr const glm::vec2 V3{52.0f, 22.0f};
    constexpr const glm::vec2 V4{34.0f, 64.0f};
    constexpr const glm::vec2 V5{73.0f, 55.0f};
    constexpr const glm::vec2 V6{71.0f, 31.0f};
    constexpr const glm::vec2 V7{72.0f, 14.0f};
    constexpr const glm::vec2 V8{92.0f, 37.0f};
    constexpr const glm::vec2 V9{59.0f, 73.0f};
    constexpr const glm::vec2 V10{79.0f, 66.0f};
    constexpr const glm::vec2 V11{52.0f, 84.0f};
    constexpr const glm::vec2 V12{26.0f, 74.0f};
    constexpr const glm::vec2 V13{29.0f, 60.0f};
    constexpr const glm::vec2 V14{20.0f, 47.0f};
    constexpr const glm::vec2 V15{15.0f, 63.0f};
    constexpr const glm::vec2 V16{23.0f, 20.0f};
    constexpr const glm::vec2 V17{12.0f, 38.0f};

    auto vertices = std::array{
         V1,  V2,  V3,
         V2,  V3,  V4,
         V3,  V4,  V5,
         V3,  V5,  V6, 
         V3,  V6,  V7,
         V5,  V6,  V8,
         V4,  V5,  V9,
         V5,  V9, V10,
         V9, V11, V12,
         V4,  V9, V12,
         V2,  V4, V13,
         V2, V13, V14,
        V13, V14, V15,
         V2, V14, V16,
        V14, V16, V17,
    };

    for (auto &vertex : vertices)
    {
        vertex -= CANVAS_SIZE / 2.0f;
        vertex /= CANVAS_SIZE / 2.0f;
    }

    return vertices;
}();

#pragma endregion vertices
