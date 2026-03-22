#pragma once

#include "Components/Component.h"
#include "Utils/Color.h"

namespace component
{

class Theme : public Component
{
  private:
    const Color color;
    const Color outlineColor;

  public:
    Theme(const Color &color, const Color &outlineColor);

    const Color &getColor() const;
    const Color &getOutlineColor() const;
};

} // namespace component
