#include "Components/PlayerShipControls.h"

#include <cassert>

#include "GameObject.h"
#include "Input.h"
#include "Utils/Constants.h"
#include "Utils/GL.h"

using namespace component;

void PlayerShipControls::initialize()
{
    Input::bindKey(Input::Action::SpeedUp, GLFW_KEY_W);
    Input::bindKey(Input::Action::TurnLeft, GLFW_KEY_A);
    Input::bindKey(Input::Action::SpeedDown, GLFW_KEY_S);
    Input::bindKey(Input::Action::TurnRight, GLFW_KEY_D);

    const auto transformOption = owner->findFirstComponentInParents<Transform>();
    assert(transformOption.has_value() && "No transform found! component::PlayerShipControls needs its node or one of "
                                          "its parents has a component::Transform");
    transform = transformOption.value();
}

void PlayerShipControls::update(float deltaTime)
{
    if (Input::getState(Input::Action::SpeedUp) == Input::State::JustPressed)
    {
        switch (speedState)
        {

        case SpeedState::Forward:
            break;
        case SpeedState::Stop:
            speedState = SpeedState::Forward;
            break;
        case SpeedState::Backward:
            speedState = SpeedState::Stop;
            break;
        }
    }

    if (Input::getState(Input::Action::SpeedDown) == Input::State::JustPressed)
    {
        switch (speedState)
        {

        case SpeedState::Forward:
            speedState = SpeedState::Stop;
            break;
        case SpeedState::Stop:
            speedState = SpeedState::Backward;
            break;
        case SpeedState::Backward:
            break;
        }
    }

    if (Input::getState(Input::Action::TurnLeft) == Input::State::JustPressed)
    {
        transform->rotate(-SHIP_ROTATION_ANGLE);
    }
    if (Input::getState(Input::Action::TurnRight) == Input::State::JustPressed)
    {
        transform->rotate(SHIP_ROTATION_ANGLE);
    }

    // Move
    const glm::vec2 direction = glm::rotate(glm::vec2(0.0f, -1.0f), transform->getRotation());

    switch (speedState)
    {
    case SpeedState::Forward:
        transform->translate(direction * deltaTime * SHIP_SPEED);
        break;
    case SpeedState::Stop:
        break;
    case SpeedState::Backward:
        transform->translate(-direction * deltaTime * SHIP_SPEED);
        break;
    }
}
