#include "Components/RigidBody.h"

#include "GameObject.h"
#include <algorithm>
#include <memory>
#include <print>

using namespace component;

RigidBody::RigidBody(bool fixed) : fixed(fixed)
{
    bodies.push_back(std::dynamic_pointer_cast<RigidBody>(shared_from_this()));
}

RigidBody::~RigidBody()
{
    const auto it = std::find_if(bodies.begin(), bodies.end(), [&](auto el) { return el.lock().get() == this;});
    if (it != bodies.end())
    {
        bodies.erase(it);
        std::println("rb destroyed");
    }
}

void RigidBody::initialize()
{
    const auto transformOption = owner.lock()->findFirstComponentInParents<Transform>();
    assert(transformOption.has_value() &&
           "No transform found! component::RigidBody needs its node or one of its parents has a component::Transform");
    transform = transformOption.value();
}

void RigidBody::update(float deltaTime)
{
    auto transform = this->transform.lock();
}
