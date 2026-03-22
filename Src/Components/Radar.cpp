#include "Components/Radar.h"

#include "GameObject.h"

using namespace component;

void Radar::initialize()
{
    const auto transformOpt = owner->findFirstComponentInParents<Transform>();
    assert(transformOpt.has_value() && "No trasnform found! component::Radar needs its node or one of its "
                                       "parents has a component::Transform");
    transform = transformOpt.value();
}

void Radar::update(float deltaTime)
{
    constexpr const float ROTATION_SPEED = glm::radians(90.0f); // rad/s
    transform->rotate(ROTATION_SPEED * deltaTime);
}
