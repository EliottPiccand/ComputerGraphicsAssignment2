#pragma once

#include "Events/Event.h"
#include "Utils/GL.h"

namespace event
{

struct Explosion : public Event
{
    const glm::vec2 position;
    const float radius;

    Explosion(const glm::vec2 &position, float radius);
};

} // namespace event
