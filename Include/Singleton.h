#pragma once

#include <memory>

#include "Components/Camera.h"
#include "Components/Water.h"

struct Singleton
{
    static inline std::weak_ptr<component::Camera> camera;
    static inline std::weak_ptr<component::Camera> uiCamera;
    static inline std::weak_ptr<component::Water> water;
};
