#pragma once

#include <functional>
#include <memory>

#include "Components/Component.h"
#include "Components/Theme.h"

namespace component
{

class Mesh : public Component
{
  public:
    using RenderCallback = std::function<void(const std::shared_ptr<Theme> theme)>;

  private:
    std::weak_ptr<Theme> theme;
    RenderCallback renderCallback;

  public:
    Mesh(RenderCallback renderCallback);

    void initialize() override;
    bool render() const override;
};

} // namespace component
