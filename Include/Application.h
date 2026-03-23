#pragma once

#include <cstdint>
#include <memory>

#include "Clock.h"
#include "GameObject.h"
#include "Utils/Time.h"
#include "Window.h"

class Application
{
  private:
    Clock clock;
    Instant lastFpsUpdate;

    std::unique_ptr<Window> window;

    std::shared_ptr<GameObject> sceneRoot;

    void update(float deltaTime);
    void render() const;

    void onResize(uint32_t width, uint32_t height);

  public:
    Application();
    void run();
};
