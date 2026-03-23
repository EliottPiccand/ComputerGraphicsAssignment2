#include "Application.h"

#include <chrono>
#include <cmath>
#include <format>
#include <numbers>
#include <ranges>
#include <string>

#include "Components/AIShipControls.h"
#include "Components/AITurretControls.h"
#include "Components/Cannonball.h"
#include "Components/Explosion.h"
#include "Components/Mesh.h"
#include "Components/PlayerShipControls.h"
#include "Components/PlayerTurretControls.h"
#include "Components/Radar.h"
#include "Components/Theme.h"
#include "Components/Trail.h"
#include "Components/TrailRenderer.h"
#include "Components/Transform.h"
#include "Components/Water.h"
#include "Events/EventQueue.h"
#include "Events/Explosion.h"
#include "Events/Fire.h"
#include "Events/RemoveGameObject.h"
#include "Input.h"
#include "Models.h"
#include "Singleton.h"
#include "Utils/Color.h"
#include "Utils/Constants.h"
#include <Utils/GL.h>
#include "Utils/Profiling.h"
#include "Utils/Random.h"

constexpr const glm::vec2 SHIP_SCALE = {50.0f, 50.0f};
constexpr const std::array SPAWN_LOCATIONS = {
    glm::vec2{0.2f, 0.2f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.8f, 0.2f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.5f, 0.5f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.2f, 0.8f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
    glm::vec2{0.8f, 0.8f} * glm::vec2{WORLD_WIDTH, WORLD_HEIGHT},
};
constexpr const size_t ENEMY_COUNT = 2;

static_assert(SPAWN_LOCATIONS.size() > ENEMY_COUNT, "Not enough spawn location to spawn the players and all enemies");

Application::Application() : lastFpsUpdate(now())
{
    Random::initialize();
    window = std::make_unique<Window>([this](uint32_t width, uint32_t height) { onResize(width, height); });
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Input::initialize(*window);
    Input::bindKey(Input::Action::ToggleFullScreen, GLFW_KEY_F11);

    auto spawnLocations = SPAWN_LOCATIONS | std::ranges::to<std::vector>();

#pragma region world

    sceneRoot = std::make_shared<GameObject>();

    Singleton::camera = sceneRoot->addComponent<component::Camera>(Window::DEFAULT_WIDTH, Window::DEFAULT_HEIGHT);
    Singleton::water = sceneRoot->addComponent<component::Water>();
    auto trailRenderer = sceneRoot->addComponent<component::TrailRenderer>();

#pragma region world_children

#pragma region player

    auto playerShipNode = sceneRoot->addChild();
    playerShipNode->addComponent<component::Transform>(
        Random::pop(spawnLocations), Random::random(0.0f, 2.0f * std::numbers::pi_v<float>), SHIP_SCALE);
    playerShipNode->addComponent<component::Theme>(PLAYER_SHIP_FILL_COLOR, PLAYER_SHIP_OUTLINE_COLOR);
    playerShipNode->addComponent<component::Mesh>(draw::polygon(SHIP_VERTICES));
    // playerShipNode->addComponent<component::Hitbox>(Args &&args...);
    playerShipNode->addComponent<component::PlayerShipControls>();
    playerShipNode->addComponent<component::Trail>(trailRenderer, FOAM_COLOR, 0.2f * glm::length(SHIP_SCALE));

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

        auto enemyShipNode = sceneRoot->addChild();
        enemyShipNode->addComponent<component::Transform>(
            Random::pop(spawnLocations), Random::random(0.0f, 2.0f * std::numbers::pi_v<float>), SHIP_SCALE);
        enemyShipNode->addComponent<component::Theme>(ENEMY_SHIP_FILL_COLOR, ENEMY_SHIP_OUTLINE_COLOR);
        enemyShipNode->addComponent<component::Mesh>(draw::polygon(SHIP_VERTICES));
        // enemyShipNode->addComponent<component::Hitbox>(Args &&args...);
        enemyShipNode->addComponent<component::AIShipControls>();
        enemyShipNode->addComponent<component::Trail>(trailRenderer, FOAM_COLOR, 0.2f * glm::length(SHIP_SCALE));

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
    // Window title update
    constexpr const Duration FPS_UPDATE_INTERVAL = std::chrono::seconds(2);

    if (now() - lastFpsUpdate > FPS_UPDATE_INTERVAL)
    {
        lastFpsUpdate = now();

        const float fps = clock.getFps();
        const std::string title = std::format("{} | {} FPS", Window::DEFAULT_TITLE, std::roundf(fps));
        window->setTitle(title);
    }

    for (const auto &rawEvent : EventQueue::popAll())
    {
        if (const auto event = dynamic_cast<event::Fire *>(rawEvent.get()))
        {
            auto cannonball = sceneRoot->addChild();
            cannonball->addComponent<component::Transform>(event->start, 0.0f, 30.0f * glm::vec2{1.0f, 1.0f});
            cannonball->addComponent<component::Theme>(CANNONBALL_FILL_COLOR, CANNONBALL_OUTLINE_COLOR);
            cannonball->addComponent<component::Mesh>(draw::polygon(CANNONBALL_VERTICES));
            cannonball->addComponent<component::Cannonball>(event->target);

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
            Singleton::camera->shake();
            Singleton::water->displaceWaterVolume(event->position, event->radius);

            auto explosion = sceneRoot->addChild();
            explosion->addComponent<component::Transform>(event->position, 0.0f, event->radius * glm::vec2{1.0f, 1.0f});
            explosion->addComponent<component::Explosion>(event->radius);

            explosion->initialize();

            continue;
        }
    }

    Input::update();

    if (Input::getState(Input::Action::ToggleFullScreen) == Input::State::JustReleased)
    {
        window->toggleFullscreen();
    }

    sceneRoot->update(deltaTime);
}

void Application::render() const
{
    glClearColor(BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, BACKGROUND_COLOR.a);
    glClear(GL_COLOR_BUFFER_BIT);

    sceneRoot->render();
}

void Application::onResize(uint32_t width, uint32_t height)
{
    Singleton::camera->onViewportResize(width, height);
}
