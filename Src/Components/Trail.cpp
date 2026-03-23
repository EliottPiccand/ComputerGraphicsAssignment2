#include "Components/Trail.h"

#include "Components/TrailRenderer.h"
#include "GameObject.h"
#include "Singleton.h"

constexpr const float TRAIL_PARTICLE_INTENTISY_DECAY = 0.5f;
constexpr const float MIN_TRAIL_STEP = 5.0f; // m

using namespace component;

Trail::Trail(std::shared_ptr<TrailRenderer> renderer, const Color &color, std::optional<float> waterDisplacementRadius)
    : renderer(renderer), color(color), waterDisplacementRadius(waterDisplacementRadius)
{
}

void Trail::initialize()
{
    const auto transformOption = owner->findFirstComponentInParents<Transform>();
    assert(transformOption.has_value() &&
           "No transform found! component::Trail needs its node or one of its parents has a component::Transform");
    transform = transformOption.value();

    renderer->registerTrail(std::dynamic_pointer_cast<Trail>(shared_from_this()));
}

void Trail::update(float deltaTime)
{
    // 1) update intensity
    for (auto &particle : particles)
    {
        particle.intensity -= deltaTime * TRAIL_PARTICLE_INTENTISY_DECAY;
    }

    // 2) remove dead ones
    while (particles.size() > 0 && particles.back().intensity <= 0.0f)
    {
        particles.popBack();
    }

    // 3) add new
    const auto position = glm::vec2(transform->resolve()[2]);
    const float distanceToPreviousTrailStep = (particles.size() > 0)
                                                  ? glm::length(position - particles.front().position)
                                                  : std::numeric_limits<float>::infinity();

    if (distanceToPreviousTrailStep >= MIN_TRAIL_STEP)
    {
        particles.pushFront(TrailParticle{
            .position = position,
            .intensity = 1.0f,
        });

        if (waterDisplacementRadius.has_value())
        {
            Singleton::water->displaceWaterVolume(position, waterDisplacementRadius.value());
        }
    }
}
