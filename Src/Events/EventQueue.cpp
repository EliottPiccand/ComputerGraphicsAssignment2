#include "Events/EventQueue.h"

std::vector<std::unique_ptr<event::Event>> EventQueue::popAll()
{
    return std::move(events);
}
