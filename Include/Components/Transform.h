#pragma once

#include <glm/glm.hpp>

#include "Components/Component.h"

namespace component
{

class Transform : public Component
{
  private:
    glm::vec2 position;
    float rotation;
    glm::vec2 scale;

  public:
    Transform(const glm::vec2 &position, float rotation, const glm::vec2 &scale);
    Transform(const glm::vec2 &position, float rotation);

    float getRotation() const;

    void setScale(float scale);
    void setPosition(const glm::vec2 &translation);

    void rotate(float by);
    void translate(const glm::vec2 &by);

    glm::mat3 resolve() const;

    bool render() const override;
};

} // namespace component
