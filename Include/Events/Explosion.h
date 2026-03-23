#pragma once

#include <glm/glm.hpp>

#include "Events/Event.h"

namespace event
{

struct Explosion : public Event
{
    const glm::vec2 position;
    const float radius;

    Explosion(const glm::vec2 &position, float radius);
};

} // namespace event
