#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <tuple>

#include "Utils/Font/PPMImage.h"

namespace font
{

class Font
{
  private:
    static inline std::unique_ptr<PPMImage> atlas;

  public:
    static void initialize();

    static std::tuple<GLuint, glm::vec2> renderTextToTexture(const std::string &text);
};

} // namespace font
