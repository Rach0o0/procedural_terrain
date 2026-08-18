#include "image_io.hpp"

#include <algorithm>
#include <cstdint> 
#include <limits>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace {

constexpr int kGrayscaleChannels = 1;

std::uint8_t toByte(float normalizedValue){
    //clamp in [0,1]
    const float clamped = std::clamp(normalizedValue, 0.0f, 1.0f);
    // multiply by 255 + add 0.5
    return static_cast<std::uint8_t>(clamped * 255.0f + 0.5f);
}

}

bool writeGrayscalePng(const Heightmap& map, const std::string& path, bool normalize){
    const std::vector<float>& values = map.values(); //reference not copy
    if (values.empty()) return false;

    float minValue = 0.0f;
    float maxValue = 1.0f;
    if (normalize){
        const auto bounds = std::minmax_element(values.begin(), values.end());
        minValue = *bounds.first;
        maxValue = *bounds.second;
    }

    const float range = maxValue - minValue;
    const float inverseRange = (range > std::numeric_limits<float>::epsilon()) ? 1.0f / range : 0.0f;

    //conversion
    std::vector<std::uint8_t> pixels(values.size());
    for (std::size_t i = 0; i < values.size(); ++i) {
        pixels[i] = toByte((values[i] - minValue) * inverseRange);
    }

    //stb call
    const int width = static_cast<int>(map.width());
    const int height = static_cast<int>(map.height());
    const int strideBytes = width * kGrayscaleChannels;

    return stbi_write_png(path.c_str(), width, height, kGrayscaleChannels, pixels.data(), strideBytes) != 0;
}