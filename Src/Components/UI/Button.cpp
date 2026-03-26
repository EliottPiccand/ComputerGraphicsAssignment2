#include "Components/UI/Button.h"

#include "Input.h"
#include "Utils/Profiling.h"

using namespace component::ui;

Button::Button(std::function<void()> callback) : callback(callback)
{
    clickable = true;
}

void Button::update(float deltaTime)
{
    ProfileScope;

    component::ui::Component::update(deltaTime);

    if (hovered && !clickCatched && Input::getState(Input::Action::UIClick) == Input::State::JustReleased)
    {
        clickCatched = true;
        callback();
    }
};
