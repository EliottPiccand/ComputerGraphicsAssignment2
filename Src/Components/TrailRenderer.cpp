#include "Components/TrailRenderer.h"

#include <GL/glew.h>

#include "Utils/Math.h"

constexpr const float TRAIL_MAX_SIZE = 60.0f; // m
constexpr const float TRAIL_MIN_SIZE = 10.0f;  // m
constexpr const float TRAIL_STEP_BLEND_FACTOR = 4.0f;  // m

using namespace component;

void TrailRenderer::registerTrail(std::shared_ptr<Trail> trail)
{
    trails.push_back(trail);
}

bool TrailRenderer::render() const
{
    for (const auto &trail : trails)
    {
        for (const auto &particle : trail->particles)
        {
            // TODO
            glColor4f(trail->color.r, trail->color.g, trail->color.b, particle.intensity / TRAIL_STEP_BLEND_FACTOR);
            glPointSize(lerp(TRAIL_MAX_SIZE, TRAIL_MIN_SIZE, particle.intensity));
            glBegin(GL_POINTS);
                glVertex2f(particle.position.x, particle.position.y);
            glEnd();
        }
    }

    return false;
}
