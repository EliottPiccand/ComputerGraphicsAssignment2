#pragma once

#include <concepts>
#include <memory>
#include <vector>

#include "Events/Event.h"

class EventQueue
{
  private:
    static inline std::vector<std::unique_ptr<event::Event>> events;

  public:
    [[nodiscard]] static std::vector<std::unique_ptr<event::Event>> popAll();

    template <std::derived_from<event::Event> EventType, typename... Args> static void post(Args... args)
    {
        events.push_back(std::make_unique<EventType>(args...));
    }
};
