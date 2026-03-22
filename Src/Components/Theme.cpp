#include "Components/Theme.h"

using component::Theme;

Theme::Theme(const Color &color, const Color &outlineColor) : color(color), outlineColor(outlineColor)
{
}

const Color &Theme::getColor() const
{
    return color;
}

const Color &Theme::getOutlineColor() const
{
    return outlineColor;
}
