#include "Components/Theme.h"

using namespace component;

constexpr const Theme::Text DEFAULT_TEXT_THEME = Theme::Text{
    .color = TEXT_COLOR,
    .scale = 1.0f,
};

Theme::Theme(const Color &fillColor, const std::optional<Color> hoveredFillColor, const std::optional<Outline> &outline,
             const std::optional<Text> &text)
    : fillColor(fillColor), hoveredFillColor(hoveredFillColor), outline(outline),
      text(text.value_or(DEFAULT_TEXT_THEME))
{
}

const Color &Theme::getFillColor() const
{
    return fillColor;
}

const std::optional<Color> &Theme::getHoveredFillColor() const
{
    return hoveredFillColor;
}

const std::optional<Theme::Outline> &Theme::getOutline() const
{
    return outline;
}

const Theme::Text &Theme::getText() const
{
    return text;
}
