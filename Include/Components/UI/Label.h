#pragma once

#include <string>

#include <GL/glew.h>

#include "Components/UI/Component.h"

namespace component::ui
{

class Label : public component::ui::Component
{
  private:
    GLuint textureId;
    std::string text;

  public:
    Label(const std::string &text);

    void setText(const std::string &text);

    void initialize() override;
    bool render() const override;
};

} // namespace component::ui
