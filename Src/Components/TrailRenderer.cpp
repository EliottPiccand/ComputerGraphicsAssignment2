#include "Components/TrailRenderer.h"

#include "Utils/GL.h"
#include "Utils/Math.h"

using namespace component;

void TrailRenderer::registerTrail(std::shared_ptr<Trail> trail)
{
    trails.push_back(trail);
}

bool TrailRenderer::render() const
{

    constexpr const float TRAIL_MAX_SIZE = 100.0f; // m
    constexpr const float TRAIL_MIN_SIZE = 20.0f; // m

    for (const auto &trail : trails)
    {
        for (const auto &particle : trail->particles)
        {
            glColor4f(trail->color.r, trail->color.g, trail->color.b, particle.intensity / 4.0f);
            glPointSize(lerp(TRAIL_MAX_SIZE, TRAIL_MIN_SIZE, particle.intensity));
            glBegin(GL_POINTS);
            glVertex2f(particle.position.x, particle.position.y);
            glEnd();
        }
    }

    return false;
}
