#include "Components/PlayerTurretControls.h"

#include <cmath>
#include <numbers>

#include "Events/EventQueue.h"
#include "Events/Fire.h"
#include "GameObject.h"
#include "Input.h"
#include "Models.h"
#include "Singleton.h"
#include "Utils/Color.h"
#include "Utils/Constants.h"

constexpr const GLfloat AIM_RAY_WIDTH = 3.0f;
constexpr const GLfloat AIM_RAY_ARROW_TIP_SIZE = 0.15f;

using namespace component;

void PlayerTurretControls::initialize()
{
    Input::bindMouseButton(Input::Action::Fire, GLFW_MOUSE_BUTTON_LEFT);
    Input::bindMouseButton(Input::Action::CancelFire, GLFW_MOUSE_BUTTON_RIGHT);

    const auto transformOption = owner->findFirstComponentInParents<Transform>();
    assert(transformOption.has_value() && "No transform found! component::PlayerTurretControls needs its node or one "
                                          "of its parents has a component::Transform");
    transform = transformOption.value();
}

void PlayerTurretControls::update(float deltaTime)
{
    const glm::mat3 resolvedTransform = transform->resolve();
    const glm::vec2 position = glm::vec2(resolvedTransform[2]);

    const glm::vec2 cursorPosition = Singleton::camera->toWorldPosition(Input::getMousePos());
    aimingValidPosition = 0.0f <= cursorPosition.x && cursorPosition.x < WORLD_WIDTH && 0.0f <= cursorPosition.y &&
                          cursorPosition.y < WORLD_HEIGHT;

    if (Input::getState(Input::Action::Fire) == Input::State::JustPressed || target.has_value())
    {
        target = std::optional(cursorPosition);
    }

    if (Input::getState(Input::Action::CancelFire) == Input::State::JustPressed)
    {
        target = std::nullopt;
    }

    if (target.has_value())
    {
        const glm::vec2 direction = glm::normalize(cursorPosition - position);
        const float angle = std::atan2(direction.y, direction.x) + std::numbers::pi_v<float> / 2.0f;
        const float currentRotation = -std::atan2(resolvedTransform[1][0], resolvedTransform[0][0]);

        transform->rotate(angle - currentRotation);
    }

    if (Input::getState(Input::Action::Fire) == Input::State::JustReleased && target.has_value())
    {
        if (aimingValidPosition)
        {
            EventQueue::post<event::Fire>(position, target.value());
        }
        target = std::nullopt;
    }
}

bool PlayerTurretControls::render() const
{
    if (target.has_value() && aimingValidPosition)
    {
        const glm::mat3 resolvedTransform = transform->resolve();
        const glm::vec2 relativeTargetPosition = glm::inverse(resolvedTransform) * glm::vec3(target.value(), 1.0f);

        draw::dashedArrow({0.0f, 0.0f}, relativeTargetPosition, AIM_RAY_COLOR, AIM_RAY_WIDTH, AIM_RAY_ARROW_TIP_SIZE);
    }

    return false;
}
