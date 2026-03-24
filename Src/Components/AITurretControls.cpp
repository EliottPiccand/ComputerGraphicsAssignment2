#include "Components/AITurretControls.h"

#include <chrono>
#include <cmath>
#include <numbers>

#include "Events/EventQueue.h"
#include "Events/Fire.h"
#include "GameObject.h"
#include "Utils/Profiling.h"
#if defined(OE_DEBUG) && defined(DISPLAY_AI_TARGETS_AIM_RAY)
#include "Models.h"
#include "Utils/Color.h"
#endif
#include "Utils/Constants.h"
#include "Utils/Random.h"
#include "Utils/Time.h"

constexpr const float TARGET_SPEED = 300.0f; // m/s
constexpr const float MIN_TARGET_NEXT_PATH_POINT_RADIUS = 200.0f;
constexpr const float TARGET_REACH_PATH_POINT_ERROR_MARGIN = 10.0f;
constexpr const Duration FIRE_TRY_INTERVAL = std::chrono::milliseconds(100);
constexpr const float FIRE_PROBABILITY_PER_TRY = 0.01f;

using namespace component;

AITurretControls::AITurretControls() : lastFireTry(now())
{
    target = {-MIN_TARGET_NEXT_PATH_POINT_RADIUS, -MIN_TARGET_NEXT_PATH_POINT_RADIUS};
    pickTargetNextPathPoint();
    target = targetNextPathPoint;
    pickTargetNextPathPoint();
}

void AITurretControls::pickTargetNextPathPoint()
{
    do
    {
        targetNextPathPoint = glm::vec2{Random::random(0.0f, WORLD_WIDTH), Random::random(0.0f, WORLD_HEIGHT)};
    } while (glm::length(target - targetNextPathPoint) < MIN_TARGET_NEXT_PATH_POINT_RADIUS);
}

void AITurretControls::initialize()
{
    const auto transformOption = owner->findFirstComponentInParents<Transform>();
    assert(transformOption.has_value() && "No transform found! component::AITurretControls needs its node or one "
                                          "of its parents has a component::Transform");
    transform = transformOption.value();
}

void AITurretControls::update(float deltaTime)
{
    ProfileScope;

    const glm::mat3 resolvedTransform = transform->resolve();
    const glm::vec2 position = glm::vec2(resolvedTransform[2]);

    // Make turret faces the target
    const glm::vec2 direction = glm::normalize(target - position);
    const float angle = std::atan2(direction.y, direction.x) + std::numbers::pi_v<float> / 2.0f;
    const float currentRotation = -std::atan2(resolvedTransform[1][0], resolvedTransform[0][0]);

    transform->rotate(angle - currentRotation);

    // Move the target
    const glm::vec2 targetDirection = glm::normalize(targetNextPathPoint - target);
    target += targetDirection * TARGET_SPEED * deltaTime;

    if (glm::length(target - targetNextPathPoint) < TARGET_REACH_PATH_POINT_ERROR_MARGIN)
    {
        pickTargetNextPathPoint();
    }

    // Fire
    if (now() - lastFireTry < FIRE_TRY_INTERVAL)
    {
        lastFireTry = now();

        if (Random::random(0.0f, 1.0f) < FIRE_PROBABILITY_PER_TRY)
        {
            EventQueue::post<event::Fire>(position, target);
        }
    }
}

#if defined(OE_DEBUG) && defined(DISPLAY_AI_TARGETS_AIM_RAY)
bool AITurretControls::render() const
{
    ProfileScope;
    ProfileScopeGPU("AITurretControls::render");

    const glm::mat3 resolvedTransform = transform->resolve();
    const glm::vec2 relativeTargetPosition = glm::inverse(resolvedTransform) * glm::vec3(target, 1.0f);

    draw::dashedArrow({0.0f, 0.0f}, relativeTargetPosition, AIM_RAY_COLOR, 3.0f, 0.0f);

    return false;
}
#endif
