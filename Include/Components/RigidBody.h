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
    ConvexPolygon collider;

    glm::vec2 velocity = {0.0f, 0.0f};
    float angularVelocity = 0.0f;

    float mass = 1.0f;
    float invMass = 1.0f;
    float inertia = 1.0f;
    float invInertia = 1.0f;
    float restitution = 0.1f;

    bool fixed;

  public:
    RigidBody(ConvexPolygon collider = {}, bool fixed = false, float mass = 1.0f, float restitution = 0.1f);
    ~RigidBody();

    void initialize() override;
    void update(float deltaTime) override;

    static void simulateAll(float deltaTime);
};

} // namespace component
