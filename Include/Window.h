#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Input;

class Window
{
  public:
    using PFN_ResizeCallback = std::function<void(uint32_t, uint32_t)>;

  private:
    struct CallbackData
    {
        PFN_ResizeCallback resizeCallback;
    };

    friend Input;

    GLFWwindow *handle;
    std::unique_ptr<CallbackData> user_data;

    int nonFullscreenPositionX;
    int nonFullscreenPositionY;
    int nonFullscreenWidth;
    int nonFullscreenHeight;
    bool isFullScreen;

  public:
    static constexpr const char *DEFAULT_TITLE = "Computer Graphics Assignment #2";
    static constexpr const uint16_t DEFAULT_WIDTH = 1280;
    static constexpr const uint16_t DEFAULT_HEIGHT = 720;

    Window(PFN_ResizeCallback resizeCallback);
    ~Window();

    [[nodiscard]] bool shouldClose() const;
    void endFrame() const;

    void setTitle(std::string title) const;
    void toggleFullscreen();

    void close();
};
