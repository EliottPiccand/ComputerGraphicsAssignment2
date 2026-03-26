#pragma once

#include <memory>
#include <optional>

#include <glm/glm.hpp>

#include "Components/Component.h"
#include "Components/Theme.h"

namespace component::ui
{

class Component : public component::Component
{
  public:
    static inline constexpr const float UI_WIDTH = 100.0f;
    static inline constexpr const float UI_HEIGHT = 100.0f;

    enum class Direction
    {
        Vertical,
        Horizontal,
    };

    enum class Align
    {
        LeftOrTop,
        Center,
        RightOrBottom,
    };

    struct Anchor
    {
        Align vertical;
        Align horizontal;
    };

  protected:
    static inline constexpr const glm::vec2 GAP = {5.0f, 5.0f}; // px

    static inline bool clickCatched = false;
    static inline bool hoverCatched = false;

    // position is set by the parent. Thus only root ui::Components must have an enforced position
    glm::vec2 position = {0.0f, 0.0f};
    // size is computed to fit the children. Thus only leaf ui::Components must have an enforced size
    glm::vec2 size = {0.0f, 0.0f};

    bool clickable = false;
    bool hovered;
    std::shared_ptr<component::Theme> theme;
    
    Direction direction = Direction::Vertical;
    Align align = Align::LeftOrTop;
    std::optional<Anchor> anchor = std::nullopt;

    std::optional<std::shared_ptr<component::ui::Component>> parent;
    std::vector<std::shared_ptr<component::ui::Component>> children;
    bool isLayoutDirty = true;

    glm::vec2 padding = {0.0f, 0.0f};

    void updateLayout();
    void requestLayoutUpdate();

  public:
    glm::vec2 getPosition() const;

    void setDirection(Direction direction);
    void setSize(const glm::vec2 &size);
    void setPadding(const glm::vec2 &padding);
    void setAlign(Align align);
    void setAnchor(const Anchor &anchor);

    void initialize() override;
    static void resetUIStates();
    void update(float deltaTime) override;
    bool render() const override;
};

} // namespace component::ui
