#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "Clock.h"
#include "Components/UI/Label.h"
#include "GameObject.h"
#include "Utils/Time.h"
#include "Window.h"

class Application
{
  private:
    Clock clock;

    std::unique_ptr<Window> window;

    std::shared_ptr<GameObject> sceneRoot;
    std::shared_ptr<GameObject> worldContainer;
    std::shared_ptr<GameObject> world;
    std::shared_ptr<GameObject> victoryMenu;
    std::shared_ptr<component::ui::Label> victoryMenuTitleLabel;
    std::vector<std::shared_ptr<GameObject>> ships;

    Instant gameStart;

    void update(float deltaTime);
    void render() const;

    void restart();
    void onResize(uint32_t width, uint32_t height);

  public:
    Application();
    void run();
};
