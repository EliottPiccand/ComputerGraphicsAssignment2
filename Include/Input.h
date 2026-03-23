#pragma once

#include <unordered_map>

#include <Utils/GL.h>
#include "Window.h"

class Input
{
  public:
    enum class Action
    {
        SpeedUp,
        SpeedDown,
        TurnLeft,
        TurnRight,
        Fire,
        CancelFire,
        ToggleFullScreen,
    };

    enum class State
    {
        JustPressed,
        HeldPressed,
        JustReleased,
        HeldReleased,
    };

  private:
    static inline GLFWwindow *window_handle = nullptr;
    static inline std::unordered_map<Action, unsigned int> binds;
    static inline std::unordered_map<Action, State> states;

  public:
    static void initialize(const Window &window);

    static void bindKey(Action action, unsigned int key);
    static void bindMouseButton(Action action, unsigned int mouseButton);
    static void update();

    [[nodiscard]] static State getState(Action action);
    [[nodiscard]] static bool isPressed(Action action);
    [[nodiscard]] static glm::vec2 getMousePos();
};
