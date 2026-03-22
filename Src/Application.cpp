#include "Application.h"

#include <chrono>
#include <memory>

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
#include "Utils/GL.h"
#include "Utils/Random.h"
#include "glm/geometric.hpp"

Application::Application() : lastFpsUpdate(now())
{
    Random::initialize();
    window = std::make_unique<Window>([this](uint32_t width, uint32_t height) { onResize(width, height); });
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Input::initialize(*window);
    Input::bindKey(Input::Action::ToggleFullScreen, GLFW_KEY_F11);

#pragma region world

    sceneRoot = std::make_shared<GameObject>();

    Singleton::camera = sceneRoot->addComponent<component::Camera>(Window::DEFAULT_WIDTH, Window::DEFAULT_HEIGHT);
    Singleton::water = sceneRoot->addComponent<component::Water>();
    auto trailRenderer = sceneRoot->addComponent<component::TrailRenderer>();

#pragma region world_children

#pragma region player

    constexpr const glm::vec2 SHIP_SCALE = {100.0f, 100.0f};

    auto playerShipNode = sceneRoot->addChild();
    playerShipNode->addComponent<component::Transform>(glm::vec2{WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f}, 0.0f,
                                                       SHIP_SCALE);
    playerShipNode->addComponent<component::Theme>(PLAYER_SHIP_FILL_COLOR, PLAYER_SHIP_OUTLINE_COLOR);
    playerShipNode->addComponent<component::Mesh>(drawPolygon(SHIP_VERTICES));
    // playerShipNode->addComponent<component::Hitbox>(Args &&args...);
    playerShipNode->addComponent<component::PlayerShipControls>();
    playerShipNode->addComponent<component::Trail>(trailRenderer, FOAM_COLOR, 0.2f * glm::length(SHIP_SCALE));

#pragma region player_children

#pragma region player_turret

    auto playerShipTurretNode = playerShipNode->addChild();
    playerShipTurretNode->addComponent<component::Transform>(glm::vec2{0.0f, -0.4f}, 0.0f);
    playerShipTurretNode->addComponent<component::Mesh>(drawPolygon(SHIP_TURRET_VERTICES));
    playerShipTurretNode->addComponent<component::PlayerTurretControls>();

#pragma endregion player_turret

    auto playerShipRadarNode = playerShipNode->addChild();
    playerShipRadarNode->addComponent<component::Transform>(glm::vec2{0.0f, 0.6f}, 0.0f);
    playerShipRadarNode->addComponent<component::Mesh>(
        [&](const std::shared_ptr<component::Theme> theme) {
            glColor4f(RADAR_CONE_COLOR.r, RADAR_CONE_COLOR.g, RADAR_CONE_COLOR.b, RADAR_CONE_COLOR.a);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glBegin(GL_POLYGON);
            for (const auto &vertex : SHIP_RADAR_CONE_VERTICES)
            {
                glVertex2f(vertex.x, vertex.y);
            }
            glEnd();

            drawPolygon(SHIP_RADAR_CENTER_VERTICES)(theme);
        });
    playerShipRadarNode->addComponent<component::Radar>();

#pragma endregion player_children

#pragma endregion player

    // for (auto &&_ : std::views::iota(0, ENEMY_COUNT)) {
    //     auto enemyShipNode = sceneRoot->addChild();
    //     enemyShipNode->addComponent<component::Transform>(Args &&args...);
    //     enemyShipNode->addComponent<component::Theme>(Args &&args...);
    //     enemyShipNode->addComponent<component::MeshRenderer>(Args &&args...);
    //     // enemyShipNode->addComponent<component::Hitbox>(Args &&args...);
    //     // enemyShipNode->addComponent<component::AIShipControls>(Args &&args...);

    //     auto enemyShipTurretNode = playerShipNode->addChild();
    //     enemyShipTurretNode->addComponent<component::Transform>(Args &&args...);
    //     enemyShipTurretNode->addComponent<component::MeshRenderer>(Args &&args...);
    //     // enemyShipTurretNode->addComponent<component::AITurretControls>(Args &&args...);

    //     auto enemyShipRadarNode = playerShipNode->addChild();
    //     enemyShipRadarNode->addComponent<component::Transform>(Args &&args...);
    //     enemyShipRadarNode->addComponent<component::MeshRenderer>(Args &&args...);
    //     // enemyShipRadarNode->addComponent<component::Radar>(Args &&args...);
    // }

    // for (auto &&_ : std::views::iota(0, OBSTACLES_COUNT)) {
    //     auto obstacleNode = sceneRoot->addChild();
    //     obstacleNode->addComponent<component::Transform>(Args &&args...);
    //     obstacleNode->addComponent<component::MeshRenderer>(Args &&args...);
    //     obstacleNode->addComponent<component::Hitbox>(Args &&args...);
    // }

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
            cannonball->addComponent<component::Mesh>(drawPolygon(CANNONBALL_VERTICES));
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
