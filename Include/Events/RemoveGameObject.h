#pragma once

#include "Events/Event.h"
#include "GameObject.h"

namespace event
{

struct RemoveGameObject : public Event
{
    const GameObjectId id;

    RemoveGameObject(const GameObjectId id);
};

} // namespace event
