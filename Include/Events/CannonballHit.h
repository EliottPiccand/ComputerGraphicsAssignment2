#pragma once

#include <glm/glm.hpp>

#include "Events/Event.h"
#include "GameObject.h"

namespace event
{

struct CannonballHit : public Event
{
    const GameObjectId cannonballId;
    const GameObjectId shipId;
    const glm::vec2 position;

    CannonballHit(GameObjectId cannonballId, GameObjectId shipId, const glm::vec2 &position);
};

} // namespace event
