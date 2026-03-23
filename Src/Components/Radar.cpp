#include "Components/Radar.h"

#include "GameObject.h"

constexpr const float ROTATION_SPEED = glm::radians(90.0f); // rad/s

using namespace component;

void Radar::initialize()
{
    const auto transformOption = owner->findFirstComponentInParents<Transform>();
    assert(transformOption.has_value() &&
           "No transform found! component::Radar needs its node or one of its parents has a component::Transform");
    transform = transformOption.value();
}

void Radar::update(float deltaTime)
{
    transform->rotate(ROTATION_SPEED * deltaTime);
}
