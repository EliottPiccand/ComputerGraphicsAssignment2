#include "Utils/Font/Font.h"

#include "Utils/Font/FontAtlas.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <print>

using namespace font;

constexpr const float CHAR_SPACING = 2.0f; // px
constexpr const float SPACE_SIZE_FACTOR = 6.0f;

void Font::initialize()
{
    atlas = std::make_unique<PPMImage>(font::FONT_ATLAS);
}

std::tuple<GLuint, glm::vec2> Font::renderTextToTexture(const std::string &text)
{
    if (text.empty())
    {
    returnEmptyTexture:
        GLuint emptyTexture;
        glGenTextures(1, &emptyTexture);
        glBindTexture(GL_TEXTURE_2D, emptyTexture);
        const std::array<uint8_t, 4> transparentPixel = {0, 0, 0, 0};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, transparentPixel.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
        return std::make_tuple(emptyTexture, glm::vec2(0.0f, 0.0f));
    }

    glm::vec2 textureSize{};

    // Compute texture size
    for (char c : text)
    {
        if (c == ' ')
        {
            textureSize += SPACE_SIZE_FACTOR * CHAR_SPACING;
            continue;
        }

        const auto it = GLYPH_MAP.find(c);
        if (it == GLYPH_MAP.end())
        {
            std::println("WARNING: character '{}' is unknow to the font, skipping", c);
            continue;
        }

        const glm::vec4 &glyph = it->second;

        // Calculate source rectangle in the atlas (pixels)
        const float glyphWidth = glyph.z * static_cast<float>(atlas->width);
        // const float glyphHeight = glyph.w * static_cast<float>(atlas->height);
        const float glyphHeight = static_cast<float>(atlas->height);

        textureSize.x += glyphWidth + CHAR_SPACING;
        textureSize.y = std::max(textureSize.y, glyphHeight);
    }

    if (textureSize.x > 0.0f)
    {
        textureSize.x -= CHAR_SPACING;
    }

    if (textureSize.x <= 0.0f || textureSize.y <= 0.0f)
    {
        goto returnEmptyTexture;
    }

    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::vector<uint8_t> texturePixels(static_cast<size_t>(textureSize.x) * static_cast<size_t>(textureSize.y) * 4,
                                       static_cast<uint8_t>(0));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(textureSize.x), static_cast<GLsizei>(textureSize.y), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, texturePixels.data());

    GLint cursorX = 0;

    for (const char c : text)
    {
        if (c == ' ')
        {
            cursorX += SPACE_SIZE_FACTOR * CHAR_SPACING;
            continue;
        }

        const auto it = GLYPH_MAP.find(c);
        if (it == GLYPH_MAP.end())
        {
            continue;
        }

        const glm::vec4 &glyph = it->second;

        const int glyphX = static_cast<int>(glyph.x * static_cast<float>(atlas->width));
        // const int glyphY = static_cast<int>(glyph.y * static_cast<float>(atlas->height));
        const int glyphY = 0;
        int glyphW = static_cast<int>(glyph.z * static_cast<float>(atlas->width));
        // int glyphH = static_cast<int>(glyph.w * static_cast<float>(atlas->height));
        int glyphH = static_cast<int>(static_cast<float>(atlas->height));

        glyphW = std::clamp(glyphW, 0, atlas->width - glyphX);
        glyphH = std::clamp(glyphH, 0, atlas->height - glyphY);

        const int startY = static_cast<int>(textureSize.y) - glyphH;

        std::vector<uint8_t> charPixels(glyphW * glyphH * 4);
        for (int y = 0; y < glyphH; y++)
        {
            for (int x = 0; x < glyphW; x++)
            {
                const int atlasX = glyphX + x;
                const int atlasY = glyphY + y;

                const size_t atlasIdx = (atlasY * atlas->width + atlasX) * 4;
                const size_t charIdx = (y * glyphW + x) * 4;

                const uint8_t r = atlas->pixels[atlasIdx + 0];
                const uint8_t g = atlas->pixels[atlasIdx + 1];
                const uint8_t b = atlas->pixels[atlasIdx + 2];

                assert(r == g && g == b && "Font can only be loaded from a gray scale image");

                charPixels[charIdx + 0] = r;
                charPixels[charIdx + 1] = g;
                charPixels[charIdx + 2] = b;
                charPixels[charIdx + 3] = r;
            }
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, cursorX, startY, glyphW, glyphH, GL_RGBA, GL_UNSIGNED_BYTE,
                        charPixels.data());
        cursorX += glyphW + CHAR_SPACING;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    return std::make_tuple(textureId, textureSize);
}
