#include "Utils/Font/PPMImage.h"

#include <cctype>
#include <charconv>
#include <optional>
#include <ranges>
#include <stdexcept>


using namespace font;

std::optional<int32_t> getNextNumber(const std::string_view data, size_t &offset)
{
    while (offset < data.size() && std::isspace(static_cast<unsigned char>(data[offset])))
    {
        offset += 1;
    }

    if (offset >= data.size())
    {
        return std::nullopt;
    }

    int32_t value;
    auto result = std::from_chars(data.data() + offset, data.data() + data.size(), value);

    if (result.ec == std::errc{} && result.ptr > data.data() + offset)
    {
        offset = static_cast<size_t>(result.ptr - data.data());
        return value;
    }
    else
    {
        // Invalid integer encountered
        return std::nullopt;
    }
}

PPMImage::PPMImage(std::string_view data)
{
    if (data.size() < 2 || data.substr(0, 2) != "P3")
    {
        throw std::runtime_error("Invalid PPM format : missing P3");
    }

    size_t offset = 2;

    const auto widthOption = getNextNumber(data, offset);
    if (!widthOption.has_value())
    {
        throw std::runtime_error("Invalid PPM format : missing width");
    }
    width = widthOption.value();

    const auto heightOption = getNextNumber(data, offset);
    if (!heightOption.has_value())
    {
        throw std::runtime_error("Invalid PPM format : missing height");
    }
    height = heightOption.value();

    const auto maxValueOption = getNextNumber(data, offset);
    if (!maxValueOption.has_value())
    {
        throw std::runtime_error("Invalid PPM format : missing maxValue");
    }
    const auto maxValue = static_cast<float>(maxValueOption.value());

    const size_t pixelCount = static_cast<size_t>(width * height);
    pixels.reserve(pixelCount * 4);
    for (const auto &&_ : std::views::iota(0uz, pixelCount))
    {
        const auto r = getNextNumber(data, offset);
        const auto g = getNextNumber(data, offset);
        const auto b = getNextNumber(data, offset);

        if (!r.has_value() || !g.has_value() || !b.has_value())
        {
            throw std::runtime_error("Invalid PPM format : missing pixel value");
        }

        pixels.push_back(static_cast<uint8_t>(static_cast<float>(r.value()) / maxValue * 255.0f));
        pixels.push_back(static_cast<uint8_t>(static_cast<float>(g.value()) / maxValue * 255.0f));
        pixels.push_back(static_cast<uint8_t>(static_cast<float>(b.value()) / maxValue * 255.0f));
        pixels.push_back(255);
    }
}
