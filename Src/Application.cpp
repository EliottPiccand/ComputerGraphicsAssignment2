#include "Application.h"

#include <chrono>
#include <memory>
#include <numbers>
#include <ranges>

#include "Components/AIShipControls.h"
#include "Components/AITurretControls.h"
#include "Components/Camera.h"
#include "Components/Cannonball.h"
#include "Components/Explosion.h"
#include "Components/Mesh.h"
#include "Components/PlayerShipControls.h"
#include "Components/PlayerTurretControls.h"
#include "Components/Radar.h"
#include "Components/RigidBody.h"
#include "Components/Theme.h"
#include "Components/Trail.h"
#include "Components/TrailRenderer.h"
#include "Components/Transform.h"
#include "Components/UI/Button.h"
#include "Components/UI/Component.h"
#include "Components/Water.h"
#include "Events/CannonballHit.h"
#include "Events/EventQueue.h"
#include "Events/Explosion.h"
#include "Events/Fire.h"
#include "Events/GameEnd.h"
#include "Events/Quit.h"
#include "Events/RemoveGameObject.h"
#include "Events/Restart.h"
#include "GLFW/glfw3.h"
#include "Input.h"
#include "Models.h"
#include "Singleton.h"
#include "Utils/Color.h"
#include "Utils/Constants.h"
#include "Utils/Font/Font.h"
#include "Utils/Profiling.h"
#include "Utils/Random.h"
#include "Window.h"

constexpr const glm::vec2 SHIP_SCALE = {50.0f, 50.0f};
constexpr const int SHIP_START_HP = 24000;
constexpr const int CANNONBALL_MIN_DAMAGE = 3000;
constexpr const int CANNONBALL_MAX_DAMAGE = 15000;
constexpr const float CANNONBALL_COLLISION_EXPLOSION_RADIUS = 50.0f;
constexpr const std::array SPAWN_LOCATIONS = {
    glm::vec2{0.2f, 0.2f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.8f, 0.2f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.5f, 0.5f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.2f, 0.8f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.8f, 0.8f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
};
constexpr const size_t ENEMY_COUNT = 2;

static_assert(SPAWN_LOCATIONS.size() > ENEMY_COUNT, "Not enough spawn location to spawn the players and all enemies");

Application::Application()
{
    ProfileScope;

    Random::initialize();
    window = std::make_unique<Window>([this](uint32_t width, uint32_t height) { onResize(width, height); });
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Input::initialize(*window);
    Input::bindKey(Input::Action::ToggleFullScreen, GLFW_KEY_F11);
    Input::bindMouseButton(Input::Action::UIClick, GLFW_MOUSE_BUTTON_1);

    font::Font::initialize();

    sceneRoot = std::make_shared<GameObject>();
    worldContainer = sceneRoot->addChild();

    restart();

#pragma region ui

    auto victoryMenu = sceneRoot->addChild();
    victoryMenu->visible = false;
    victoryMenu->active = false;
    Singleton::uiCamera = victoryMenu->addComponent<component::Camera>(
        glm::vec4{0.0f, component::ui::Component::UI_WIDTH, component::ui::Component::UI_HEIGHT, 0.0f},
        Window::DEFAULT_WIDTH, Window::DEFAULT_HEIGHT);
    victoryMenu->addComponent<component::Theme>(UI_FILL_COLOR, std::optional(UI_HOVERED_COLOR),
                                                std::optional(component::Theme::Outline{
                                                    .color = UI_OUTLINE_COLOR,
                                                    .width = 3.0f,
                                                }),
                                                std::optional(component::Theme::Text{
                                                    .color = TEXT_COLOR,
                                                    .scale = 0.1f,
                                                }));
    this->victoryMenu = victoryMenu;

    {
        const auto [framebufferWidth, framebufferHeight] = window->getFramebufferSize();
        onResize(framebufferWidth, framebufferHeight);
    }

    auto victoryMenuComponent = victoryMenu->addComponent<component::ui::Component>();
    victoryMenuComponent->setDirection(component::ui::Component::Direction::Vertical);
    victoryMenuComponent->setPadding(glm::vec2{4.0f, 5.0f});
    victoryMenuComponent->setAlign(component::ui::Component::Align::Center);
    victoryMenuComponent->setAnchor(component::ui::Component::Anchor{
        .vertical = component::ui::Component::Align::Center,
        .horizontal = component::ui::Component::Align::Center,
    });

    auto victoryMenuTitle = victoryMenu->addChild();
    victoryMenuTitleLabel = victoryMenuTitle->addComponent<component::ui::Label>("Victory Menu");

    auto victoryMenuButtonContainer = victoryMenu->addChild();
    victoryMenuButtonContainer->addComponent<component::ui::Component>()->setDirection(
        component::ui::Component::Direction::Horizontal);

    auto victoryMenuButtonRestart = victoryMenuButtonContainer->addChild();
    auto btn1 =
        victoryMenuButtonRestart->addComponent<component::ui::Button>([]() { EventQueue::post<event::Restart>(); });
    btn1->setPadding({2.0f, 5.0f});
    victoryMenuButtonRestart->addChild()->addComponent<component::ui::Label>("Restart");

    auto victoryMenuButtonQuit = victoryMenuButtonContainer->addChild();
    auto btn2 = victoryMenuButtonQuit->addComponent<component::ui::Button>([]() { EventQueue::post<event::Quit>(); });
    btn2->setPadding({2.0f, 5.0f});
    victoryMenuButtonQuit->addChild()->addComponent<component::ui::Label>("Quit");

#pragma endregion ui

    sceneRoot->initialize();
}

void Application::run()
{
    while (!window->shouldClose())
    {
        const float deltaTime = clock.tick();
        if (deltaTime > 1.0f)
        {
            continue;
        }

        update(deltaTime);
        render();

        window->endFrame();

        ProfilingEndFrame;
    }
}

void Application::update(float deltaTime)
{
    ProfileScope;

    for (const auto &rawEvent : EventQueue::popAll())
    {
        if (const auto event = dynamic_cast<event::CannonballHit *>(rawEvent.get()))
        {
            if (const auto cannonballOption = sceneRoot->getGameObject(event->cannonballId); cannonballOption.has_value())
            {
                cannonballOption.value()->detach();
            }
            EventQueue::post<event::Explosion>(event->position, CANNONBALL_COLLISION_EXPLOSION_RADIUS);

            const auto hpIt = shipHitPoints.find(event->shipId);
            if (hpIt != shipHitPoints.end() && hpIt->second > 0)
            {
                const int damage = Random::randint(CANNONBALL_MIN_DAMAGE, CANNONBALL_MAX_DAMAGE);
                hpIt->second = std::max(0, hpIt->second - damage);

                if (hpIt->second == 0)
                {
                    const auto player = playerShip.lock();
                    const bool destroyedPlayerShip = player && event->shipId == player->getId();

                    if (const auto shipOption = sceneRoot->getGameObject(event->shipId); shipOption.has_value())
                    {
                        // Remove the whole node so its rigid body no longer participates in physics.
                        shipOption.value()->detach();
                    }

                    if (destroyedPlayerShip)
                    {
                        EventQueue::post<event::GameEnd>(false);
                    }
                    else
                    {
                        bool allEnemiesDestroyed = true;
                        for (const GameObjectId enemyId : enemyShipIds)
                        {
                            const auto enemyHpIt = shipHitPoints.find(enemyId);
                            if (enemyHpIt == shipHitPoints.end() || enemyHpIt->second > 0)
                            {
                                allEnemiesDestroyed = false;
                                break;
                            }
                        }

                        if (allEnemiesDestroyed)
                        {
                            EventQueue::post<event::GameEnd>(true);
                        }
                    }
                }
            }

            continue;
        }

        if (const auto event = dynamic_cast<event::Fire *>(rawEvent.get()))
        {
            auto world = this->world.lock();
            auto cannonball = world->addChild();
            cannonball->addComponent<component::Transform>(event->start, 0.0f, 30.0f * glm::vec2{1.0f, 1.0f});
            cannonball->addComponent<component::Theme>(CANNONBALL_FILL_COLOR, CANNONBALL_OUTLINE_COLOR);
            cannonball->addComponent<component::Mesh>(draw::polygon(CANNONBALL_VERTICES));
            cannonball->addComponent<component::RigidBody>(
                component::ConvexPolygon(CANNONBALL_VERTICES.begin(), CANNONBALL_VERTICES.end()), false, 0.5f, 0.1f);
            cannonball->addComponent<component::Cannonball>(event->target, event->shooterId);

            cannonball->initialize();

            continue;
        }

        if (const auto event = dynamic_cast<event::RemoveGameObject *>(rawEvent.get()))
        {
            const auto gameObjectOption = sceneRoot->getGameObject(event->id);
            if (gameObjectOption.has_value())
            {
                gameObjectOption.value()->detach();
            }

            continue;
        }

        if (const auto event = dynamic_cast<event::Explosion *>(rawEvent.get()))
        {
            Singleton::camera.lock()->shake();
            Singleton::water.lock()->displaceWaterVolume(event->position, event->radius);

            auto explosion = world.lock()->addChild();
            explosion->addComponent<component::Transform>(event->position, 0.0f, event->radius * glm::vec2{1.0f, 1.0f});
            explosion->addComponent<component::Explosion>(event->radius);

            explosion->initialize();

            continue;
        }

        if (const auto _ = dynamic_cast<event::Quit *>(rawEvent.get()))
        {
            window->close();
            return;
        }

        if (const auto _ = dynamic_cast<event::Restart *>(rawEvent.get()))
        {
            restart();
        }

        if (const auto event = dynamic_cast<event::GameEnd *>(rawEvent.get()))
        {
            if (gameEnded)
            {
                continue;
            }
            gameEnded = true;

            auto victoryMenu = this->victoryMenu.lock();
            victoryMenu->visible = true;
            victoryMenu->active = true;
            victoryMenuTitleLabel.lock()->setText(event->victory ? "You Won :)" : "You lost :(");

            for (auto &ship : ships)
            {
                if (const auto s = ship.lock())
                {
                    s->active = false;
                }
            }
        }
    }

    Input::update();

    if (Input::getState(Input::Action::ToggleFullScreen) == Input::State::JustReleased)
    {
        window->toggleFullscreen();
    }

    component::ui::Component::resetUIStates();
    sceneRoot->update(deltaTime);
    component::RigidBody::simulateAll(deltaTime);
}

void Application::render() const
{
    ProfileScope;

    glClearColor(BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, BACKGROUND_COLOR.a);
    glClear(GL_COLOR_BUFFER_BIT);

    sceneRoot->render();
}

void Application::restart()
{
    gameEnded = false;

    auto victoryMenu = this->victoryMenu.lock();

    if (victoryMenu.get() != nullptr)
    {
        victoryMenu->visible = false;
        victoryMenu->active = false;
    }

    auto old_world = this->world.lock();

    if (old_world.get() != nullptr)
    {
        old_world->detach();
    }

    ships.clear();
    shipHitPoints.clear();
    enemyShipIds.clear();
    playerShip = std::weak_ptr<GameObject>();

    auto spawnLocations = SPAWN_LOCATIONS | std::ranges::to<std::vector>();

#pragma region world

    auto worldContainer = this->worldContainer.lock();
    auto world = worldContainer->addChild();
    this->world = world;

    Singleton::camera = world->addComponent<component::Camera>(
        component::Camera::getWorldLBRT(Window::DEFAULT_WIDTH, Window::DEFAULT_HEIGHT), Window::DEFAULT_WIDTH,
        Window::DEFAULT_HEIGHT);
    Singleton::water = world->addComponent<component::Water>();
    auto trailRenderer = world->addComponent<component::TrailRenderer>();

#pragma region world_children

#pragma region player

    auto playerShipNode = world->addChild();
    playerShipNode->addComponent<component::Transform>(
        Random::pop(spawnLocations), Random::random(0.0f, 2.0f * std::numbers::pi_v<float>), SHIP_SCALE);
    playerShipNode->addComponent<component::Theme>(PLAYER_SHIP_FILL_COLOR, std::nullopt,
                                                   std::optional(component::Theme::Outline{
                                                       .color = PLAYER_SHIP_OUTLINE_COLOR,
                                                       .width = SHIP_OUTLINE_WIDTH,
                                                   }));
    playerShipNode->addComponent<component::Mesh>(draw::polygon(SHIP_VERTICES));
    playerShipNode->addComponent<component::RigidBody>(
        component::ConvexPolygon(SHIP_VERTICES.begin(), SHIP_VERTICES.end()), false, 2.0f, 0.05f);
    playerShipNode->addComponent<component::PlayerShipControls>();
    playerShipNode->addComponent<component::Trail>(trailRenderer, FOAM_COLOR, 0.2f * glm::length(SHIP_SCALE));

    ships.push_back(playerShipNode);
    playerShip = playerShipNode;
    shipHitPoints[playerShipNode->getId()] = SHIP_START_HP;

#pragma region player_children

#pragma region player_turret

    auto playerShipTurretNode = playerShipNode->addChild();
    playerShipTurretNode->addComponent<component::Transform>(glm::vec2{0.0f, -0.4f}, 0.0f);
    playerShipTurretNode->addComponent<component::PlayerTurretControls>();
    playerShipTurretNode->addComponent<component::Mesh>(draw::polygon(SHIP_TURRET_VERTICES));

#pragma endregion player_turret

#pragma region player_radar

    auto playerShipRadarNode = playerShipNode->addChild();
    playerShipRadarNode->addComponent<component::Transform>(glm::vec2{0.0f, 0.6f}, 0.0f);
    playerShipRadarNode->addComponent<component::Mesh>([&](const std::shared_ptr<component::Theme> theme) {
        glColor4f(RADAR_CONE_COLOR.r, RADAR_CONE_COLOR.g, RADAR_CONE_COLOR.b, RADAR_CONE_COLOR.a);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_POLYGON);
        for (const auto &vertex : SHIP_RADAR_CONE_VERTICES)
        {
            glVertex2f(vertex.x, vertex.y);
        }
        glEnd();

        draw::polygon(SHIP_RADAR_CENTER_VERTICES)(theme);
    });
    playerShipRadarNode->addComponent<component::Radar>();

#pragma endregion player_radar

#pragma endregion player_children

#pragma endregion player

#pragma region enemies

    for (const auto &&_ : std::views::iota(0uz, ENEMY_COUNT))
    {

#pragma region enemy

        auto enemyShipNode = world->addChild();
        enemyShipNode->addComponent<component::Transform>(
            Random::pop(spawnLocations), Random::random(0.0f, 2.0f * std::numbers::pi_v<float>), SHIP_SCALE);
        enemyShipNode->addComponent<component::Theme>(ENEMY_SHIP_FILL_COLOR, std::nullopt,
                                                      std::optional(component::Theme::Outline{
                                                          .color = ENEMY_SHIP_OUTLINE_COLOR,
                                                          .width = SHIP_OUTLINE_WIDTH,
                                                      }));
        enemyShipNode->addComponent<component::Mesh>(draw::polygon(SHIP_VERTICES));
        enemyShipNode->addComponent<component::RigidBody>(
            component::ConvexPolygon(SHIP_VERTICES.begin(), SHIP_VERTICES.end()), false, 2.0f, 0.05f);
        enemyShipNode->addComponent<component::AIShipControls>();
        enemyShipNode->addComponent<component::Trail>(trailRenderer, FOAM_COLOR, 0.2f * glm::length(SHIP_SCALE));

        ships.push_back(enemyShipNode);
        shipHitPoints[enemyShipNode->getId()] = SHIP_START_HP;
        enemyShipIds.insert(enemyShipNode->getId());

#pragma region enemy_children

#pragma region enemy_turret

        auto enemyShipTurretNode = enemyShipNode->addChild();
        enemyShipTurretNode->addComponent<component::Transform>(glm::vec2{0.0f, -0.4f}, 0.0f);
        enemyShipTurretNode->addComponent<component::AITurretControls>();
        enemyShipTurretNode->addComponent<component::Mesh>(draw::polygon(SHIP_TURRET_VERTICES));

#pragma endregion enemy_turret

#pragma region enemy_radar

        auto enemyShipRadarNode = enemyShipNode->addChild();
        enemyShipRadarNode->addComponent<component::Transform>(glm::vec2{0.0f, 0.6f}, 0.0f);
        enemyShipRadarNode->addComponent<component::Mesh>([&](const std::shared_ptr<component::Theme> theme) {
            glColor4f(RADAR_CONE_COLOR.r, RADAR_CONE_COLOR.g, RADAR_CONE_COLOR.b, RADAR_CONE_COLOR.a);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glBegin(GL_POLYGON);
            for (const auto &vertex : SHIP_RADAR_CONE_VERTICES)
            {
                glVertex2f(vertex.x, vertex.y);
            }
            glEnd();

            draw::polygon(SHIP_RADAR_CENTER_VERTICES)(theme);
        });
        enemyShipRadarNode->addComponent<component::Radar>();

#pragma endregion enemy_radar

#pragma endregion enemy_children

#pragma endregion enemy
    }

#pragma endregion enemies

#pragma endregion world_children

#pragma endregion world

    world->initialize();

    {
        const auto [framebufferWidth, framebufferHeight] = window->getFramebufferSize();
        onResize(framebufferWidth, framebufferHeight);
    }

    gameStart = now();
}

void Application::onResize(uint32_t width, uint32_t height)
{
    if (auto camera = Singleton::camera.lock())
    {
        camera->onViewportResize(width, height);
        camera->lrbt = component::Camera::getWorldLBRT(width, height);
    }

    if (auto uiCamera = Singleton::uiCamera.lock())
    {
        uiCamera->onViewportResize(width, height);
    }
}
