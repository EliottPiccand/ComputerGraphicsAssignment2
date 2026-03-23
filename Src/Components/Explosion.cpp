#include "Components/Explosion.h"

#include "Events/EventQueue.h"
#include "Events/RemoveGameObject.h"
#include "GameObject.h"
#include "Models.h"
#include "Utils/GL.h"
#include "Utils/Random.h"

constexpr const float MAX_RADIUS = 100.0f;
constexpr const float EXPANSION_RATE = 300.0f; // m/s

using namespace component;

Explosion::Explosion(float radius)
    : radius(radius), rotations([] {
          std::array<float, COLORS.size()> rotations;

          for (auto &rotation : rotations)
          {
              rotation = Random::random(0.0f, 360.0f);
          }

          return rotations;
      }())
{
}

void Explosion::initialize()
{
    const auto transformOption = owner->findFirstComponentInParents<Transform>();
    assert(transformOption.has_value() &&
           "No theme found! component::Explosion needs its node or one of its parents has a component::Transform");

    transform = transformOption.value();
}

void Explosion::update(float deltaTime)
{
    radius += EXPANSION_RATE * deltaTime;
    transform->setScale(radius);

    if (radius > MAX_RADIUS)
    {
        EventQueue::post<event::RemoveGameObject>(getOwner()->getId());
    }
}

bool Explosion::render() const
{
    float scale = 1.0f;
    for (const auto &[baseColor, rotation] : std::views::zip(COLORS, rotations))
    {
        glPushMatrix();
        glRotatef(rotation, 0.0f, 0.0f, 1.0f);
        glScalef(scale, scale, 1.0f);

        const auto color = glm::mix(EXPLOSION_FLASH_COLOR, baseColor, radius / MAX_RADIUS);

        glColor3f(color.r, color.g, color.b);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_TRIANGLES);
        for (const auto &vertex : EXPLOSION_VERTICES)
        {
            glVertex2f(vertex.x, vertex.y);
        }
        glEnd();

        scale /= 2.0f;
    }

    for (const auto &_ : COLORS)
    {
        glPopMatrix();
    }

    return false;
}
