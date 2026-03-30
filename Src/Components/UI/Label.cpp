#include "Components/UI/Label.h"

#include "Components/UI/Component.h"
#include "Utils/Font/Font.h"
#include "Utils/Profiling.h"

using namespace component::ui;

Label::Label(const std::string &text) : text(text)
{
}

void Label::setText(const std::string &text)
{
    ProfileScope;

    this->text = text;

    const auto [textureId_, size_] = font::Font::renderTextToTexture(text);
    textureId = textureId_;
    size = size_ * theme.lock()->getText().scale + 2.0f * padding;

    requestLayoutUpdate();
}

void Label::initialize()
{
    component::ui::Component::initialize();

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    setText(text);
}

bool Label::render() const
{
    ProfileScope;
    ProfileScopeGPU("Label::render");

    // component::ui::Component::render();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    const auto &color = theme.lock()->getText().color;
    glColor4f(color.r, color.g, color.b, color.a);

    const auto textPosition = getPosition() + padding;
    const auto textSize = size - 2.0f * padding;

    glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(textPosition.x, textPosition.y);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(textPosition.x + textSize.x, textPosition.y);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(textPosition.x + textSize.x, textPosition.y + textSize.y);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(textPosition.x, textPosition.y + textSize.y);
        glEnd();
    glDisable(GL_TEXTURE_2D);

    return false;
}
