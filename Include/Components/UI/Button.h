#pragma once

#include <functional>

#include "Components/UI/Component.h"

namespace component::ui
{

class Button : public component::ui::Component
{
  private:
    std::function<void()> callback;

  public:
    Button(std::function<void()> callback);

    void update(float deltaTime) override;
};

} // namespace component::ui
