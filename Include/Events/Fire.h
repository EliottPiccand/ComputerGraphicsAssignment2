#pragma once

#include <glm/glm.hpp>

#include "Events/Event.h"

namespace event
{

struct Fire : public Event
{
    const glm::vec2 start;
    const glm::vec2 target;

    Fire(const glm::vec2 &start, const glm::vec2 &target);
};

} // namespace event
