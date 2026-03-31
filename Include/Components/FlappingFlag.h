#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "Components/Component.h"
#include "Components/Theme.h"

namespace component
{

class FlappingFlag : public Component
{
  private:
    std::weak_ptr<Theme> theme;
    std::vector<glm::vec2> baseMesh;
    std::vector<glm::vec2> mesh;
    float t;

  public:
    void initialize() override;
    void update(float deltaTime) override;
    bool render() const override;
};

} // namespace component
