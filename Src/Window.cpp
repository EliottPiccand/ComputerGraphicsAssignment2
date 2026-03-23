#include "Window.h"

#include <cassert>
#include <cstddef>
#include <format>
#include <stdexcept>

static void glfwErrorCallback(int code, const char *description)
{
    const std::string message = std::format("GLFW error ({}) : {}", code, description);
    throw std::runtime_error(message);
}

Window::Window(PFN_ResizeCallback resizeCallback) : isFullScreen(false)
{
    glfwSetErrorCallback(glfwErrorCallback);
    glfwInit();

    handle = glfwCreateWindow(static_cast<int>(DEFAULT_WIDTH), static_cast<int>(DEFAULT_HEIGHT), DEFAULT_TITLE, nullptr,
                              nullptr);
    glfwMakeContextCurrent(handle);

    user_data = std::make_unique<CallbackData>(resizeCallback);
    glfwSetWindowUserPointer(handle, user_data.get());

    glfwSetFramebufferSizeCallback(handle, [](GLFWwindow *window, int width, int height) {
        const CallbackData *user_data = static_cast<CallbackData *>(glfwGetWindowUserPointer(window));
        user_data->resizeCallback(width, height);
    });
}

Window::~Window()
{
    glfwTerminate();
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(handle) == GLFW_TRUE;
}

void Window::endFrame() const
{
    glfwSwapBuffers(handle);
    glfwPollEvents();
}

void Window::setTitle(std::string title) const
{
    glfwSetWindowTitle(handle, title.c_str());
}

void Window::toggleFullscreen()
{
    isFullScreen = !isFullScreen;

    if (isFullScreen)
    {
        // get current monitor
        GLFWmonitor *currentMonitor = nullptr;

        int currentWindowX, currentWindowY;
        glfwGetWindowPos(handle, &currentWindowX, &currentWindowY);

        int count;
        GLFWmonitor **monitors = glfwGetMonitors(&count);
        for (size_t i = 0; i < count; i++)
        {
            int monitorX, monitorY, width, height;
            glfwGetMonitorWorkarea(monitors[i], &monitorX, &monitorY, &width, &height);

            if ((monitorX <= currentWindowX && currentWindowX < monitorX + width) &&
                (monitorY <= currentWindowY && currentWindowY < monitorY + height))
            {
                currentMonitor = monitors[i];
                break;
            }
        }

        assert(currentMonitor != nullptr && "failed to retrive current monitor");

        // save current state
        int width, height;
        glfwGetWindowSize(handle, &width, &height);

        nonFullscreenPositionX = currentWindowX;
        nonFullscreenPositionY = currentWindowY;
        nonFullscreenWidth = width;
        nonFullscreenHeight = height;

        // set fullscreen
        const GLFWvidmode *videoMode = glfwGetVideoMode(currentMonitor);
        assert(videoMode != nullptr && "failed to retrieve current video mode");

        glfwSetWindowMonitor(handle, currentMonitor, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
    }
    else
    {
        glfwSetWindowMonitor(handle, nullptr, nonFullscreenPositionX, nonFullscreenPositionY, nonFullscreenWidth,
                             nonFullscreenHeight, GLFW_DONT_CARE);
    }
}
