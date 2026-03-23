#include "Components/PlayerTurretControls.h"

#include <cmath>
#include <numbers>
#include <optional>

#include "Components/Transform.h"
#include "Events/EventQueue.h"
#include "Events/Fire.h"
#include "GameObject.h"
#include "Input.h"
#include "Models.h"
#include "Singleton.h"
#include "Utils/Color.h"
#include "Utils/Constants.h"

using namespace component;

void PlayerTurretControls::initialize()
{
    Input::bindMouseButton(Input::Action::Fire, GLFW_MOUSE_BUTTON_LEFT);
    Input::bindMouseButton(Input::Action::CancelFire, GLFW_MOUSE_BUTTON_RIGHT);

    const auto transformOpt = owner->findFirstComponentInParents<Transform>();
    assert(transformOpt.has_value() && "No trasnform found! component::PlayerTurretControls needs its node or one of "
                                       "its parents has a component::Transform");
    transform = transformOpt.value();
}

void PlayerTurretControls::update(float deltaTime)
{
    glm::mat3 resolvedTransform = transform->resolve();
    glm::vec2 position = glm::vec2(resolvedTransform[2]);

    glm::vec2 cursorPosition = Singleton::camera->toWorldPosition(Input::getMousePos());
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

    if (target.has_value()) {
        glm::vec2 direction = glm::normalize(cursorPosition - position);
        float angle = std::atan2(direction.y, direction.x) + std::numbers::pi_v<float> / 2.0f;
        float currentRotation = -std::atan2(resolvedTransform[1][0], resolvedTransform[0][0]);

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

bool PlayerTurretControls::render() const {
    constexpr const GLfloat AIM_RAY_WIDTH = 3.0f; 

    if (target.has_value() && aimingValidPosition) {
        glm::mat3 resolvedTransform = transform->resolve();
        glm::vec2 relativeTargetPosition = glm::inverse(resolvedTransform) * glm::vec3(target.value(), 1.0f);

        draw::dashedArrow({0.0f, 0.0f}, relativeTargetPosition, AIM_RAY_COLOR, AIM_RAY_WIDTH, 0.15f);
    }

    return false;
}
