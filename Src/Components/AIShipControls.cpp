#include "Components/AIShipControls.h"

#include <cassert>
#include <chrono>

#include "GameObject.h"
#if defined(_DEBUG) && defined(DISPLAY_AI_TARGETS)
#include "Models.h"
#endif
#include "Utils/Constants.h"
#include "Utils/Random.h"

constexpr const float MIN_NEXT_TARGET_RADIUS = 200.0f;   // m
constexpr const float MAX_NEXT_TARGET_RADIUS = 600.0f;   // m
constexpr const float TARGET_REACH_ERROR_MARGIN = 20.0f; // m
constexpr const Duration TURN_MIN_INTERVAL = std::chrono::milliseconds(200);

constexpr const std::array PATH_POINTS = {
    glm::vec2{0.1f, 0.1f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.3f, 0.1f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.5f, 0.1f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.7f, 0.1f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.9f, 0.1f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},

    glm::vec2{0.1f, 0.3f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.3f, 0.3f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.5f, 0.3f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.7f, 0.3f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.9f, 0.3f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},

    glm::vec2{0.1f, 0.5f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.3f, 0.5f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.5f, 0.5f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.7f, 0.5f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.9f, 0.5f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},

    glm::vec2{0.1f, 0.7f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.3f, 0.7f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.5f, 0.7f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.7f, 0.7f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.9f, 0.7f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},

    glm::vec2{0.1f, 0.9f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.3f, 0.9f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.5f, 0.9f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.7f, 0.9f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.9f, 0.9f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
};

using namespace component;

AIShipControls::AIShipControls() : lastTurn(now() - TURN_MIN_INTERVAL)
{
}

void AIShipControls::pickTarget()
{
    const auto position = glm::vec2(transform->resolve()[2]);
    float distance;

    do
    {
        target = Random::range<glm::vec2>(PATH_POINTS);
        distance = glm::length(target - position);
    } while (MIN_NEXT_TARGET_RADIUS <= distance && distance <= MAX_NEXT_TARGET_RADIUS);
}

void AIShipControls::initialize()
{
    const auto transformOption = owner->findFirstComponentInParents<Transform>();
    assert(transformOption.has_value() && "No transform found! component::AIShipControls needs its node or one of "
                                          "its parents has a component::Transform");
    transform = transformOption.value();

    pickTarget();
}

void AIShipControls::update(float deltaTime)
{
    const glm::vec2 position = transform->resolve()[2];
    const glm::vec2 direction = glm::rotate(glm::vec2(0.0f, -1.0f), transform->getRotation());

    if (glm::length(position - target) <= TARGET_REACH_ERROR_MARGIN)
    {
        pickTarget();
    }

    const glm::vec2 targetDirection = glm::normalize(target - position);
    const float angleToTarget = glm::orientedAngle(direction, targetDirection);

    if (std::abs(angleToTarget) > SHIP_ROTATION_ANGLE / 2.0f && now() - lastTurn > TURN_MIN_INTERVAL)
    {
        lastTurn = now();
        transform->rotate(SHIP_ROTATION_ANGLE * glm::sign(angleToTarget));
    }

    // Move
    const glm::vec2 updatedDirection = glm::rotate(glm::vec2(0.0f, -1.0f), transform->getRotation());

    switch (speedState)
    {
    case SpeedState::Forward:
        transform->translate(updatedDirection * deltaTime * SHIP_SPEED);
        break;
    case SpeedState::Stop:
        break;
    case SpeedState::Backward:
        transform->translate(-updatedDirection * deltaTime * SHIP_SPEED);
        break;
    }
}

#if defined(_DEBUG) && defined(DISPLAY_AI_TARGETS)

bool AIShipControls::render() const
{
    const glm::mat3 resolvedTransform = transform->resolve();
    const glm::vec2 relativeTargetPosition = glm::inverse(resolvedTransform) * glm::vec3(target, 1.0f);

    draw::dashedArrow({0.0f, 0.0f}, relativeTargetPosition, AIM_RAY_COLOR, 3.0f, 0.0f);

    return false;
}

#endif
