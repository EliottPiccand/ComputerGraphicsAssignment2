#include "Components/FlappingFlag.h"

#include <cmath>
#include <numbers>

#include "GameObject.h"
#include "Models.h"

using namespace component;

constexpr const float FLAG_LENGTH = 50.0f;
constexpr const float STEP = 1.0f;
constexpr const float ANGLE = glm::radians(20.0f);

constexpr const float PI_2 = 2.0f * std::numbers::pi_v<float>;
constexpr const float OMEGA_T = PI_2 * 0.8f;
constexpr const float OMEGA_Y = PI_2 * 0.9f / FLAG_LENGTH;
constexpr const float AMPLITUDE = 10.0f;

void FlappingFlag::initialize()
{
    const auto themeOption = owner.lock()->findFirstComponentInParents<Theme>();
    assert(themeOption.has_value() &&
           "No theme found! component::FlappingFlag needs its node or one of its parents has a component::Theme");

    theme = themeOption.value();

    glm::vec2 pos = {0.0f, 0.0f};
    baseMesh.push_back(pos);
    while (pos.y < FLAG_LENGTH)
    {
        pos.y += STEP;
        baseMesh.push_back(pos);
    }
    while (pos.y > 0.0f)
    {
        pos.y -= STEP;
        pos.x += STEP * std::sinf(ANGLE);
        baseMesh.push_back(pos);
    }

    mesh = baseMesh;
}

void FlappingFlag::update(float deltaTime)
{
    t += deltaTime;
    while (t > PI_2)
    {
        t -= PI_2;
    }

    auto n = mesh.size();
    for (size_t i = 1; i < n - i; i++)
    {
        float offset = AMPLITUDE * baseMesh[i].y / FLAG_LENGTH * std::sinf(OMEGA_T * t + OMEGA_Y * baseMesh[i].y);
        mesh[i].x = baseMesh[i].x + offset;
        mesh[n-i].x = baseMesh[n-i].x + offset;
    }
}

bool FlappingFlag::render() const
{
    draw::polygon(mesh)(theme.lock());
    return false;
}
