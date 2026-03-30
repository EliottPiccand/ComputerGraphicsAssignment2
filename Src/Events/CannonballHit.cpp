#include "Events/CannonballHit.h"

using namespace event;

CannonballHit::CannonballHit(GameObjectId cannonballId, GameObjectId shipId, const glm::vec2 &position)
    : cannonballId(cannonballId), shipId(shipId), position(position)
{
}
