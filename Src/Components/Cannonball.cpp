#include "Components/Cannonball.h"

#include <numbers>

#include "Events/EventQueue.h"
#include "Events/Explosion.h"
#include "Events/RemoveGameObject.h"
#include "GameObject.h"

constexpr const float EXPLOSION_RADIUS = 50.0f; // m

using namespace component;

Cannonball::Cannonball(const glm::vec2 &target) : target(target)
{
}

void Cannonball::initialize()
{
    const auto transformOpt = owner->findFirstComponentInParents<Transform>();
    assert(transformOpt.has_value() && "No transform found! component::Cannonball needs its node or one of its "
                                       "parents has a component::Transform");
    transform = transformOpt.value();
}

void Cannonball::update(float deltaTime)
{
    constexpr const float SPEED = 500.0f; // m/s
    constexpr const float REACH_ERROR_MARGIN = 5.0f; // m

    glm::mat3 resolvedTransform = transform->resolve();
    glm::vec2 position = glm::vec2(resolvedTransform[2]);

    if (glm::length(target - position) < REACH_ERROR_MARGIN) {
        EventQueue::post<event::RemoveGameObject>(owner->getId());
        EventQueue::post<event::Explosion>(target, EXPLOSION_RADIUS);
        return;
    }

    glm::vec2 direction = glm::normalize(target - position);
    float angle = std::atan2(direction.y, direction.x) + std::numbers::pi_v<float> / 2.0f;
    float currentRotation = -std::atan2(resolvedTransform[1][0], resolvedTransform[0][0]);

    transform->rotate(angle - currentRotation);

    transform->translate(direction * SPEED * deltaTime);
}
