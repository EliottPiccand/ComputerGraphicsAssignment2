#include "Events/Fire.h"

using namespace event;

Fire::Fire(const glm::vec2 &start, const glm::vec2 &target, GameObjectId shooterId)
    : start(start), target(target), shooterId(shooterId)
{
}
