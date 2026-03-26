#pragma once

#include <GL/glew.h>

#include <cstdint>
#include <string_view>
#include <vector>

namespace font
{

class PPMImage
{
  public:
    int width;
    int height;
    std::vector<uint8_t> pixels;

    PPMImage(std::string_view content);
};

} // namespace font
