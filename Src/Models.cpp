#include "Models.h"

#include <GL/glew.h>
#include <glm/gtx/rotate_vector.hpp>

void draw::dashedArrow(const glm::vec2 &start, const glm::vec2 &end, const Color &color, float width, float tipSize)
{
    constexpr const glm::vec2 ARROW_RATIO = {1.0f, 2.0f};

    glColor4f(color.r, color.g, color.b, color.a);
    glLineWidth(width);

    glPushAttrib(GL_ENABLE_BIT);

        glLineStipple(1, 0xFF00);
        glEnable(GL_LINE_STIPPLE);

        glBegin(GL_LINES);
            glVertex2f(start.x, start.y);
            glVertex2f(end.x, end.y);
        glEnd();

    glPopAttrib();

    const auto direction = glm::normalize(end - start);
    const auto arrowStart = end + (-direction + glm::rotate(direction, glm::radians(-90.0f))) * ARROW_RATIO * tipSize;
    const auto arrowEnd   = end + (-direction + glm::rotate(direction, glm::radians( 90.0f))) * ARROW_RATIO * tipSize;

    glBegin(GL_LINE_STRIP);
        glVertex2f(arrowStart.x, arrowStart.y);
        glVertex2f(end.x, end.y);
        glVertex2f(arrowEnd.x, arrowStart.y);
    glEnd();
}
