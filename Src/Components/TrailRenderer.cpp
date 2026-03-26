#include "Components/TrailRenderer.h"

#include <GL/glew.h>

#include "Utils/Math.h"
#include "Utils/Profiling.h"

constexpr const float TRAIL_MAX_SIZE = 60.0f;         // m
constexpr const float TRAIL_MIN_SIZE = 10.0f;         // m
constexpr const float TRAIL_STEP_BLEND_FACTOR = 4.0f; // m
constexpr const float TRAIL_PARTICLE_INTENTISY_DECAY = 0.5f;

using namespace component;

void TrailRenderer::registerTrail(std::shared_ptr<Trail> trail)
{
    trails.push_back(trail);
}

void TrailRenderer::update(float deltaTime)
{
    for (auto &trail : trails)
    {
        for (auto &particle : trail->particles)
        {
            particle.intensity -= deltaTime * TRAIL_PARTICLE_INTENTISY_DECAY;
        }

        // 2) remove dead ones
        while (trail->particles.size() > 0 && trail->particles.back().intensity <= 0.0f)
        {
            trail->particles.popBack();
        }
    }
}

bool TrailRenderer::render() const
{
    ProfileScope;
    ProfileScopeGPU("TrailRenderer::render");

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
