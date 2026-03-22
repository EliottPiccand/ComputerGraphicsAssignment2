#include "Components/Mesh.h"

#include <cassert>

#include "GameObject.h"

using namespace component;

Mesh::Mesh(Mesh::RenderCallback renderCallback) : renderCallback(renderCallback)
{
}

void Mesh::initialize()
{
    const auto themeOpt = owner->findFirstComponentInParents<Theme>();
    assert(themeOpt.has_value() &&
           "No theme found! component::Mesh needs its node or one of its parents has a component::Theme");

    theme = themeOpt.value();
}

bool Mesh::render() const
{
    renderCallback(theme);
    return false;
}
