#include "Components/Cannonball.h"

#include <numbers>

#include "Events/EventQueue.h"
#include "Events/Explosion.h"
#include "Events/RemoveGameObject.h"
#include "GameObject.h"
#include "Utils/Profiling.h"

constexpr const float EXPLOSION_RADIUS = 50.0f;  // m
constexpr const float SPEED = 500.0f;            // m/s
constexpr const float REACH_ERROR_MARGIN = 5.0f; // m

using namespace component;

Cannonball::Cannonball(const glm::vec2 &target, GameObjectId shooterId) : target(target), shooterId(shooterId)
{
}

GameObjectId Cannonball::getShooterId() const
{
    return shooterId;
}

void Cannonball::initialize()
{
    const auto transformOption = owner.lock()->findFirstComponentInParents<Transform>();
    assert(
        transformOption.has_value() &&
        "No transform found! component::Cannonball needs its node or one of its  parents has a component::Transform");
    transform = transformOption.value();
}

void Cannonball::update(float deltaTime)
{
    ProfileScope;

    auto transform = this->transform.lock();

    const glm::mat3 resolvedTransform = transform->resolve();
    const glm::vec2 position = glm::vec2(resolvedTransform[2]);

    if (glm::length(target - position) < REACH_ERROR_MARGIN)
    {
        EventQueue::post<event::RemoveGameObject>(owner.lock()->getId());
        EventQueue::post<event::Explosion>(target, EXPLOSION_RADIUS);
        return;
    }

    const glm::vec2 direction = glm::normalize(target - position);
    const float angle = std::atan2(direction.y, direction.x) + std::numbers::pi_v<float> / 2.0f;
    const float currentRotation = -std::atan2(resolvedTransform[1][0], resolvedTransform[0][0]);

    transform->rotate(angle - currentRotation);

    transform->translate(direction * SPEED * deltaTime);
}
