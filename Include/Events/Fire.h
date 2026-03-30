#pragma once

#include <glm/glm.hpp>

#include "Events/Event.h"
#include "GameObject.h"

namespace event
{

struct Fire : public Event
{
    const glm::vec2 start;
    const glm::vec2 target;
    const GameObjectId shooterId;

    Fire(const glm::vec2 &start, const glm::vec2 &target, GameObjectId shooterId);
};

} // namespace event
