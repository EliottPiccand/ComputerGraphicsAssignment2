#pragma once

#include <memory>

#include "Components/Camera.h" // NOLINT
#include "Components/Water.h"

struct Singleton {
    static inline std::shared_ptr<component::Camera> camera;
    static inline std::shared_ptr<component::Water> water;
};
