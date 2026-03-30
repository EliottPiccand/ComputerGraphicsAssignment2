#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Components/Component.h"
#include "Components/Transform.h"

namespace component
{

using ConvexPolygon = std::vector<glm::vec2>;

// https://timallanwheeler.com/blog/2024/08/01/2d-collision-detection-and-resolution/
class RigidBody : public Component
{
  private:
    static inline std::vector<std::weak_ptr<RigidBody>> bodies;

    struct KineticState
    {
        glm::vec2 position;
        glm::vec2 velocity;
        float rotation;
        float angular_velocity;
    };

    KineticState previousState;

    std::weak_ptr<Transform> transform;
    bool fixed;

  public:
    RigidBody(bool fixed = false);
    ~RigidBody();

    void initialize() override;
    void update(float deltaTime) override;
};

} // namespace component
