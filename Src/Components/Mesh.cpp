#include "Components/Mesh.h"

#include <cassert>

#include "GameObject.h"
#include "Utils/Profiling.h"

using namespace component;

Mesh::Mesh(Mesh::RenderCallback renderCallback) : renderCallback(renderCallback)
{
}

void Mesh::initialize()
{
    const auto themeOption = owner->findFirstComponentInParents<Theme>();
    assert(themeOption.has_value() &&
           "No theme found! component::Mesh needs its node or one of its parents has a component::Theme");

    theme = themeOption.value();
}

bool Mesh::render() const
{
    ProfileScope;
    ProfileScopeGPU("Mesh::render");

    renderCallback(theme);
    return false;
}
