#pragma once

#include <optional>

#include <GL/glew.h>

#include "Components/Component.h"
#include "Utils/Color.h"

namespace component
{

class Theme : public Component
{
  public:
    struct Outline
    {
        const Color color;
        const GLfloat width;
    };

    struct Text
    {
        const Color color;
        const float scale;
    };

  private:
    const Color fillColor;
    const std::optional<Color> hoveredFillColor;
    const std::optional<Outline> outline;
    const Text text;

  public:
    Theme(const Color &fillColor, const std::optional<Color> hoveredFillColor = std::nullopt,
          const std::optional<Outline> &outline = std::nullopt, const std::optional<Text> &text = std::nullopt);

    const Color &getFillColor() const;
    const std::optional<Color> &getHoveredFillColor() const;
    const std::optional<Outline> &getOutline() const;
    const Text &getText() const;
};

} // namespace component
