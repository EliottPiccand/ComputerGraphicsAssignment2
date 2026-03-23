#include "Components/Transform.h"

#include <numbers>

#include <GL/glew.h>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "GameObject.h"

using namespace component;

Transform::Transform(const glm::vec2 &position, float rotation, const glm::vec2 &scale)
    : position(position), rotation(rotation), scale(scale)
{
}

Transform::Transform(const glm::vec2 &position, float rotation) : Transform(position, rotation, glm::vec2{1.0f, 1.0f})
{
}

float Transform::getRotation() const
{
    return rotation;
}

void Transform::setScale(float scale)
{
    this->scale = glm::vec2{scale, scale};
}

void Transform::rotate(float by)
{
    constexpr const float PI_2 = 2.0f * std::numbers::pi_v<float>;

    rotation += by;

    while (rotation > PI_2)
    {
        rotation -= PI_2;
    }

    while (rotation < 0.0f)
    {
        rotation += PI_2;
    }
}

void Transform::translate(const glm::vec2 &by)
{
    position += by;
}

glm::mat3 Transform::resolve() const
{
    glm::mat3 transform;
    const auto ownerParentOpt = owner->getParent();

    if (ownerParentOpt.has_value())
    {
        const auto previousTransformOpt = ownerParentOpt.value()->findFirstComponentInParents<Transform>();
        if (previousTransformOpt.has_value())
        {
            transform = previousTransformOpt.value()->resolve();
        }
        else
        {
            transform = glm::mat3(1.0f);
        }
    }
    else
    {
        transform = glm::mat3(1.0f);
    }

    transform = glm::translate(transform, position);
    transform = glm::rotate(transform, rotation);
    transform = glm::scale(transform, scale);

    return transform;
}

bool Transform::render() const
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(position.x, position.y, 0.0f);
    glRotatef(glm::degrees(rotation), 0.0f, 0.0f, 1.0f);
    glScalef(scale.x, scale.y, 1.0f);

    return true;
}
