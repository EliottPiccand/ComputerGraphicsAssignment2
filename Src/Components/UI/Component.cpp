#include "Components/UI/Component.h"

#include <algorithm>

#include "GameObject.h"
#include "Input.h"
#include "Singleton.h"
#include "Utils/Profiling.h"

using namespace component::ui;

void Component::updateLayout()
{
    if (children.size() > 0)
    {
        glm::vec2 offset = padding;
        size = padding * 2.0f;
        for (auto &child : children)
        {
            child->updateLayout();
            glm::vec2 childSize = child->size;

            switch (direction)
            {
            case Direction::Vertical: {
                child->position.y = offset.y;
                offset.y += GAP.y + childSize.y;
                size.x = std::max(size.x, childSize.x + 2.0f * padding.x);
                size.y += GAP.y + childSize.y;
            }
            break;
            case Direction::Horizontal: {
                child->position.x = offset.x;
                offset.x += GAP.x + childSize.x;
                size.x += GAP.x + childSize.x;
                size.y = std::max(size.y, childSize.y + 2.0f * padding.y);
            }
            break;
            }
        }

        if (children.size() > 0)
        {
            switch (direction)
            {
            case Direction::Vertical:
                size.y -= GAP.y;
                break;
            case Direction::Horizontal:
                size.x -= GAP.x;
                break;
            }
        }

        for (auto &child : children)
        {
            switch (direction)
            {
            case Direction::Vertical:
                switch (align)
                {
                case Align::LeftOrTop:
                    child->position.x = padding.x;
                    break;
                case Align::Center:
                    child->position.x = (size.x - child->size.x) / 2.0f;
                    break;
                case Align::RightOrBottom:
                    child->position.x = size.x - child->size.x - padding.x;
                    break;
                }
                break;
            case Direction::Horizontal:
                switch (align)
                {
                case Align::LeftOrTop:
                    child->position.y = padding.y;
                    break;
                case Align::Center:
                    child->position.y = (size.y - child->size.y) / 2.0f;
                    break;
                case Align::RightOrBottom:
                    child->position.y = size.y - child->size.y - padding.y;
                    break;
                }
                break;
            }
        }
    }
    
    if (anchor.has_value())
    {
        assert(!parent.has_value() && "anchor property can only be used on root ui components");

        switch (anchor->horizontal) {
        case Align::LeftOrTop:
            break;
        case Align::Center:
            position.x = (ui::Component::UI_WIDTH - size.x) / 2.0f;
            break;
        case Align::RightOrBottom:
            position.x = ui::Component::UI_WIDTH - size.x;
            break;
        }

        switch (anchor->vertical) {
        case Align::LeftOrTop:
            break;
        case Align::Center:
            position.y = (ui::Component::UI_HEIGHT - size.y) / 2.0f;
            break;
        case Align::RightOrBottom:
            position.y = ui::Component::UI_HEIGHT - size.y;
            break;
        }
    }

    isLayoutDirty = false;
}

void Component::requestLayoutUpdate()
{
    isLayoutDirty = true;
    if (parent.has_value())
    {
        parent.value()->requestLayoutUpdate();
    }
}

glm::vec2 Component::getPosition() const
{
    glm::vec2 pos = position;

    if (parent.has_value())
    {
        pos += parent.value()->getPosition();
    }

    return pos;
}

void Component::setDirection(Component::Direction direction)
{
    this->direction = direction;
    requestLayoutUpdate();
}

void Component::setSize(const glm::vec2 &size)
{
    this->size = size;
    requestLayoutUpdate();
}

void Component::setPadding(const glm::vec2 &padding)
{
    this->padding = padding;
    size = padding * 2.0f;
    requestLayoutUpdate();
}

void Component::setAlign(Align align)
{
    this->align = align;
    requestLayoutUpdate();
}

void Component::setAnchor(const Anchor &anchor)
{
    this->anchor = std::optional(anchor);
    requestLayoutUpdate();
}

void Component::initialize()
{
    const auto themeOption = owner->findFirstComponentInParents<component::Theme>();
    assert(themeOption.has_value() &&
           "No theme found! component::ui::Component needs its node or one of its parents has a component::Theme");
    theme = themeOption.value();

    parent = owner->findFirstComponentInParents<component::ui::Component>(false);
    if (parent.has_value())
    {
        parent.value()->children.push_back(std::dynamic_pointer_cast<ui::Component>(shared_from_this()));
    }
}

void Component::resetUIStates()
{
    hoverCatched = false;
    clickCatched = false;
}

void Component::update(float deltaTime)
{
    ProfileScope;

    if (isLayoutDirty && !parent.has_value())
    {
        updateLayout();
    }

    const auto &mousePosRelative = Singleton::uiCamera->toWorldPosition(Input::getMousePos()) - getPosition();
    hovered = 0.0f <= mousePosRelative.x && mousePosRelative.x <= size.x && 0.0f <= mousePosRelative.y &&
              mousePosRelative.y <= size.y && !hoverCatched;

    if (hovered && clickable)
    {
        hoverCatched = true;
    }
}

bool Component::render() const
{
    ProfileScope;
    ProfileScopeGPU("Component::render");


    const auto &baseFillColor = parent.has_value() ? TRANSPARENT_COLOR : theme->getFillColor();
    const auto &fillColor = (clickable && hovered) ? theme->getHoveredFillColor().value_or(baseFillColor) : baseFillColor;
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor4f(fillColor.r, fillColor.g, fillColor.b, fillColor.a);

    const auto pos = getPosition();
    glRectf(pos.x, pos.y, pos.x + size.x, pos.y + size.y);

    const auto &outline = theme->getOutline();
    if (outline.has_value())
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(outline->width);
        glColor4f(outline->color.r, outline->color.g, outline->color.b, outline->color.a);
        glRectf(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
    }

    return false;
}
