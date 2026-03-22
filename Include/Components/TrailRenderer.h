#pragma once

#include <memory>
#include <vector>

#include "Components/Component.h"
#include "Components/Trail.h"

namespace component
{

class TrailRenderer : public Component
{
  private:
    std::vector<std::shared_ptr<Trail>> trails;

  public:
    void registerTrail(std::shared_ptr<Trail> trail);

    bool render() const override;
};

} // namespace component
