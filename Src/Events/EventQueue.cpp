#include "Events/EventQueue.h"

#include <utility>

std::vector<std::unique_ptr<event::Event>> EventQueue::popAll()
{
    return std::move(events);
}
