#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <unordered_set>
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
    std::weak_ptr<GameObject> worldContainer;
    std::weak_ptr<GameObject> world;
    std::weak_ptr<GameObject> victoryMenu;
    std::weak_ptr<component::ui::Label> victoryMenuTitleLabel;
    std::vector<std::weak_ptr<GameObject>> ships;
    std::weak_ptr<GameObject> playerShip;

    std::unordered_map<GameObjectId, int> shipHitPoints;
    std::unordered_set<GameObjectId> enemyShipIds;
    bool gameEnded = false;

    Instant gameStart;

    void update(float deltaTime);
    void render() const;

    void restart();
    void onResize(uint32_t width, uint32_t height);

  public:
    Application();
    void run();
};
